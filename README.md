# Unicode Patch for Anvil

An [Anvil](https://github.com/happy-dwarf-team/anvil) plugin for Europa Universalis IV
1.37.5 (Steam, Windows x64): a port of [EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch)
by VulonLok (MIT) from its EU4DLL-style `VERSION.dll` loader to Anvil, so that Chinese,
Japanese and Korean players no longer choose between their text patch and Anvil
([anvil#104](https://github.com/happy-dwarf-team/anvil/issues/104)).

**Status: draft.** It builds and its unit tests pass. The ported search finds the
province finder's English names, accented or not, as the game does. The localisation
import keeps the game's text in UTF-8, as the original does, and the ported text blocks
draw it, Chinese, Japanese and Korean too when a font holds those glyphs (from a mod; see
[CJK fonts](#cjk-fonts-with-utf8_localization)); it stays off (`utf8_localization`) until
the map labels, the editor and the original's own font atlases are ported. `search` stays off by default until the diplomacy country list has been checked in the
game (see [State](#state)). Not affiliated with Paradox Interactive.

## Where it lives and why

Not a standard plugin in the Anvil repository. Anvil's design §4 keeps in the core only
what must work before any plugin loads or needs a view across all plugins, and keeps
game knowledge that only some plugins need (GUI, text) in those plugins. Standard plugins
are the few that every plugin may build on (`anvil-game`, `anvil-overlay`) or that every
player benefits from (`anvil-log-throttle`). This port is none of those:

- it changes how every text of the game is laid out, drawn and typed, which only players
  who read or write CJK (or other non-Latin) text need;
- it follows an active upstream (several releases a month) and is large: 289 symbols and
  about 170 changes;
- its licence and credit belong to the original's author.

So it is a plugin of its own, in this repository, which builds the original's text logic
from the pinned `upstream/` submodule instead of copying it. Upstream's own repository
(an Anvil target next to its proxy) stays the preferred long-term home; that needs the
author's agreement.

## Licence

MIT (`LICENSE`), the original's copyright notice and this port's. The original is MIT
© 2026 VulonLok, which allows this port with the notice kept. Its parts that the port
builds: utfcpp (BSL-1.0) and phrase-pinyin-data (MIT), both through `upstream/`.
MinHook, the MASM stubs and the proxy loader are not used.

## What it does so far

Four features of the original.

### Search

**Chinese and pinyin search** in the diplomacy country list
and the province finder (characters, full pinyin, initials, mixed input, one typo in long
pinyin, optional fuzzy pinyin), as the original does it:

| Symbol | The game | The plugin |
| --- | --- | --- |
| `find_country_name` `0x17061A0` | substring search of a country list entry | for the three callers the original changes (`native_search.cpp`), the original's matching; every other caller gets the game's function |
| `find_province_distance` `0x171F880` | fuzzy distance in the province finder | the same for its two callers |
| 10 × `search_lower_call_*`, 9 × `search_unaccent_call_*` | lower-case and strip Latin-1 accents from search keys in place, which breaks UTF-8 | NOPs, as in the original; the original's matching is case- and accent-insensitive itself |

The original's search rejects text that is not UTF-8. Without the localisation import
the game's names are Windows-1252, and the typed query always is (the editor and IME are
not ported), so `src/game_text.cpp` decodes text that is not valid UTF-8 as Windows-1252
before it reaches the original's code; valid UTF-8, such as an imported name, passes
unchanged.

### Localisation import (`utf8_localization`, off by default)

The game reads its localisation files as UTF-8 and converts every value to Windows-1252
before it stores it; a character without a Windows-1252 form becomes `?`, which is why
Chinese, Japanese and Korean translations cannot load. The original replaces that
function (`src/localisation.cpp`):

| Symbol | The game | The plugin |
| --- | --- | --- |
| `import_legacy_localization` `0x16FD650` | converts key and value to Windows-1252 (`0x17059E0`), then registers them | valid UTF-8 is registered as it is, EU4DLL escapes of old translation packs are decoded to UTF-8, anything else is rejected and logged (the first 20, then only counted) |
| `parse_utf8_yaml` `0x16FC3D0` | parses a localisation file | a valid UTF-8 file without the signature gets one in a copy, as in the original |
| `register_imported_text` `0x16FA8D0` | the game's registration of one value | called by the plugin, not changed |

At the main menu the plugin logs how many values it kept, decoded and rejected.

### Text drawing (with `utf8_localization`)

The game walks text byte by byte and takes each byte as the index into its font's 256
glyphs, so without these blocks UTF-8 accents show as two wrong letters ("Ã–stra
GÃ¶taland", [the test before them](#in-game-test-of-the-localisation-import)). 36 of the
original's per-glyph blocks (`text_hooks.asm`, `layout_hooks.asm`, `popup_hooks.asm`) are
Anvil block replacements in `src/text_blocks.cpp`, each doing what the original's stub
does with registers, flags and continuations:

| Game routine | Blocks |
| --- | --- |
| main text `0x15988E0` (labels, lists, tooltips) | copy, measure, format, draw, two icon ends, the 32000-byte input limit |
| button text `0x1596660` | copy, format, measure, advance, draw format, draw, two icon ends |
| width routines `0x159B470`, `0x159B7C0`, `0x159EC30`, `0x159F510` | format, measure, split, advance, icon end |
| popup text `0x159C600` | copy (3), format, measure, advance, draw format, draw, icon end |

They take one UTF-8 character and its glyph instead of one byte (`src/text_drawing.cpp`).
A character above U+00FF is drawn from the font's own glyph for it when the font has one
(below); otherwise from the font's glyph at its Windows-1252 byte (U+2013 at 0x96, where
the game's fonts hold it), or `?`, as the game itself shows it. The index the game's code
sees after a block (kerning, line breaks) is always that byte, never above 0xFF, so the
original's blocks for its own glyphs (shaped paragraphs, atlas pages, line breaks and
kerning above U+00FF) are not needed; the price is that Chinese text wraps only at spaces. Text that is not UTF-8, such as names from scripts and saves,
stays one byte per glyph, so both encodings draw side by side. The decoder is the
plugin's own: the original's throws a C++ exception on every byte that is not UTF-8,
which here would be every accented name of a save in every frame.

### CJK fonts (with `utf8_localization`)

The game's font loader drops every glyph above U+00FF (`cmp edi, 0xFF`), and the Steam
1.37.5 fonts have none. As in the original, the limit becomes U+10FFFF together with the
loader's lookup, allocation and store blocks (`font_hooks.asm`) and the original's glyph
registry (`glyph_registry.cpp`), which keeps those glyphs apart from the font's table of
256 (alone, the limit would let the loader write past it); `destroy_font_table` releases
them. So a font that holds Chinese, Japanese or Korean glyphs on its one texture page
draws them with the game's own renderer.

Such a font comes from a mod for now. The original's `fontpack` tool (built here) draws
the characters of a UTF-8 file with DirectWrite from the system's fonts (Microsoft YaHei UI
first) into atlases, and `tools/make-font-mod.py` lays them out under the game's font
names:

```
build/Release/fontpack.exe <text in UTF-8> <atlas folder>
python -I tools/make-font-mod.py <atlas folder> <mod folder>
```

The original instead draws the game's fonts itself at start-up and adds missing glyphs
while the game runs (its font atlases, texture uploads and D3D9 hooks), so no mod is
needed; that is not ported ([Next steps](#next-steps)).

Settings (`settings.toml`, the overlay's panel): `search`, `utf8_localization`,
`typo_tolerance`, `fuzzy_pinyin`, which replace the original's `config.ini`. A `pinyin.txt` in the plugin
folder adds readings, as in the original.

`symbols.toml` declares every address of the original (91 blocks with their 113
continuations, 56 functions, 26 byte patches), the game functions the plugin calls
(`register_imported_text`, `repeat_text`, `append_text`), the copy buffer of the main
text routine and the game's operator new (values of their blocks' bytes), generated by `tools/gen-symbols.py` from the
original's hook tables, so that later features only add changes. `anvil check . --exe`
passes on the Steam `eu4.exe` (SHA-256 `9AD3EFE1…D771A`); `symbols.lock` is committed.

## State

| Step (plan in the EU4 engine lab, `docs/anvil-unicode-patch-research.md`) | State |
| --- | --- |
| Repository, licence, `plugin.toml`, `settings.toml`, CI workflow | written |
| `symbols.toml` with all 289 symbols | `anvil check --exe` OK |
| Search: 2 hooks, 19 byte patches | builds; `anvil check` of the built plugin folder OK |
| Localisation import: 2 hooks, 1 call | builds; off by default (`utf8_localization`) |
| Text drawing: 36 blocks | on with `utf8_localization` |
| Font loader: the U+10FFFF limit, 3 blocks, 1 hook, the glyph registry; `fontpack` and `tools/make-font-mod.py` | on with `utf8_localization` |
| The original's unit tests (UTF-8, EU4DLL escapes, search, pinyin, YAML), `tests/port_tests.cpp`, `tests/localisation_tests.cpp` and `tests/text_drawing_tests.cpp` | `ctest` 9/9 (Release, MSVC 2022) |
| In-game test of the province finder (2.11.2026, Anvil 1.6.0, Tongs 0.24.0, Russia 1719 save, English) | the regression found on 1.11 is fixed; see below |
| In-game test of the localisation import (2.11.2026, the same conditions) | 259096 values kept in UTF-8, none rejected; see below |
| In-game test of the text drawing (2.11.2026, the same conditions) | accented localisation drawn right; no measurable cost per frame; see below |
| In-game test of CJK fonts from a mod (2.11.2026) | Chinese, Japanese and Korean drawn right in the UI; map labels not; see below |
| In-game test of the diplomacy country list | not done |
| The rest of the original (script and save lexer, map labels, its own font atlases, editor and IME, dates, Rich Presence, save names) | not started; see [Next steps](#next-steps) |
| Name order (a separate plugin with `affects_gameplay = true`) and the mod registry fix | not started; the mod registry fix decides which mods load and does not belong here |

### In-game test of the search

The province finder (`minimap_panel/button_goto`, "Exact matches only" off), the same queries
without the plugin, with the build before the fix (`633ff58`) and with the fix (`587ab6e`),
on 2.11.2026 with the display on (the game drew 79–83 frames per second in every run).
Screenshots, logs and conditions are in the EU4 engine lab (`docs/anvil-unicode-patch-research.md`).

| Query | Without the plugin | Before the fix | With the fix |
| --- | --- | --- | --- |
| `ostergotland` | Östergötland, Gotland, Easter Island, Shetland, Östra Götaland, Västerbotten, Ingermanland, Ostróda | Gotland, Easter Island, Shetland, Ingermanland, Vogtland, Sutherland, Teotitlan, Stargard | Östergötland, Östra Götaland, Gotland, Easter Island, Shetland, Østjylland, Västerbotten, Ingermanland |
| `MOSKV` | Moskva, Mosul, Moshi, Morea, York, Pskov, Minsk, Mokha | the same | the same |
| `götaland` | — | — | Östra Götaland, Västra Götaland, Gotland, Åland, Lolland, Vogtland, Holland, Holland |

- The cause: the original's search throws on text that is not valid UTF-8, and the
  detours caught it and dropped the candidate, so every accented province went to the end
  of the finder and every accented country left the diplomacy search (2816 and 1704
  warnings in the log of the run before the fix, none with it).
- The first test (1.11.2026) ran with the PC's display asleep. Its `MOSKV` result
  (no Moskva in the first 8) did not repeat with the display on; the accented names did.
- With the fix, the distance-ranked part of the list differs from the game's in places
  (Östra Götaland 2nd instead of 5th, Østjylland in the first 8), probably because the original's
  Latin keys fold more letters than the game's accent stripping (not checked in the game's
  code).
- Not checked yet: the diplomacy country list, Chinese and pinyin search (they need the
  localisation import), and a comparison with the original patch itself.

### In-game test of the localisation import

Three starts on 2.11.2026 with the conditions above (Anvil 1.6.0, Tongs 0.24.0, Russia 1719
save, English; 74–80 frames per second): the plugin's defaults, `utf8_localization` with
`search`, and `utf8_localization` alone.

- The import kept all 259096 values of the English localisation in UTF-8: none rejected,
  none with EU4DLL escapes, every file already signed. With the defaults the plugin
  changes nothing (`0 change(s)` in `anvil.log`).
- Text from the localisation is in UTF-8 in memory and drawn wrong: the province view's
  area name "Östra Götaland" reads as "Ã–stra GÃ¶taland" through Tongs, and the game draws
  "Ã– stra GÃ¶taland" and "VÃ¤stra GÃ¶taland". With the defaults it is drawn right.
- Province and capital names (Östergötland, Linköping) stay Windows-1252 and are drawn
  right: in a loaded game they come from the save and the culture name lists, which the
  game reads through its script lexer, not through the localisation.
- The province finder, "Exact matches only" off:

| Query | Defaults | `utf8_localization` and `search` | `utf8_localization` alone |
| --- | --- | --- | --- |
| `ostergotland` | the game's list (Östergötland, Gotland, Easter Island, Shetland, Östra Götaland, Västerbotten, Ingermanland, Ostróda) | the list of the search fix (Östergötland, Östra Götaland, Gotland, …) | Östergötland, Gotland, Easter Island, Shetland, Västerbotten, Ingermanland, Ostróda, Vogtland: Östra Götaland is gone |
| `MOSKV` | — | the game's list | — |
| `götaland` | — | Östra Götaland, Västra Götaland, Gotland, Åland, Lolland, Vogtland, Holland, Holland | Gotland, Lolland, Vogtland, Holland, Holland, Norrland, Värmland, Jämtland: both Götalands are gone |

So the import needs the ported search: the game's own lower-casing and accent stripping
break UTF-8 names, and the import should switch the search on with it.

### In-game test of the text drawing

Three starts on 2.11.2026 with the conditions above (63–78 frames per second): two with
`utf8_localization` and `search` (`profile = true`, closed through the game's Exit Game so
that Anvil wrote its call counts), one with the defaults.

- The area name "Östra Götaland", UTF-8 from the localisation, is drawn right in the
  province view's header and tab, in the province finder ("Östra Götaland", "Västra
  Götaland") and in the tooltip "Östergötland is located in the Östra Götaland area",
  where the Windows-1252 province name and the UTF-8 area name share one string. The
  province finder gives the same lists as before.
- The English interface shows no difference on the screens checked: the top bar, the
  outliner ("Içel"), the province view, map tooltips ("Hälsingland"), map labels.
- The blocks ran about 2,900 and 3,150 times per frame (32.3 million calls in 11,250
  frames, 18.9 million in 6,000; 85% in the main text routine). The popup blocks ran 0 times:
  the tooltips seen went through the main text routine.
- Main-thread CPU per frame on the open province view, three 30-second samples with the
  blocks (12.16, 12.28, 12.42 ms) against two without (12.08, 12.93 ms): no difference
  within the spread. Anvil's budget for a block replacement is 25 ns a call, so the
  estimate is about 0.1 ms per frame (about 1% of the main thread), not checked apart.

### In-game test of CJK fonts from a mod

Five starts on 2.11.2026 (Anvil 1.6.0, Tongs 0.24.0), each with a private game profile
(`-userdir`, the player's own profile unchanged) that enables one test mod: the original's
probe localisation (`fixtures/localisation/eu4_unicode_probe_l_english.yml`: the main
menu's buttons, FRA, CAS, ENG, TUR and others in Chinese, Japanese and Korean) and fonts
from `fontpack` (339 characters, 22 UI fonts), laid out as `tools/make-font-mod.py` does.

- With `utf8_localization`: the font loader kept 6,808 glyphs above U+00FF in 46 font
  tables; the main menu's buttons read 单人游戏 UTF8, 中文与 English, 日本語 한국어, 设置
  UTF8 (yellow, from its colour code), 退出游戏; in the single-player lobby the selected
  nation reads 奥斯曼 and the bookmark tooltip "奥斯曼 is an interesting choice during this
  period" mixes it with English, both right.
- The map label of the same country shows Latin-1 letters in place of 奥斯曼: the map
  labels' blocks are not ported.
- Without the plugin's changes the menu is in English: the game leaves out the probe file,
  which has no UTF-8 signature (the localisation import gives it one).
- Cost at the main menu (165 frames per second): the text blocks ran about 0.2 times a frame
  there (the game keeps laid-out menu text), the font loader blocks 15,812 times while the
  fonts loaded. Main-thread CPU per frame 1.14 ms with the plugin, 0.97 and about 1.04 ms
  without: no difference that the blocks could explain at that call rate.
- Starting the private profile straight into the save (`continue=`) crashed twice in
  `pops_api.dll` (a null read, no plugin on the stack); starting it in the menu did not.
  The first start ran with the display asleep (8 frames per second) and does not count.

## Next steps

A plan only; none of it is ported. The original's order, from the lab's analysis:

1. **Map labels** (`map_hooks.asm`, 26 blocks, and the country and province label shaping):
   country names on the map come from the localisation and are still read byte by byte.
   With them and the editor below, `utf8_localization` can default to on together with
   `search`.
2. **Script and save lexer** (`0x170CD10`, `0x170CA20`, `0x170D1D0`, `0x170E700`,
   `0x170EBF0`): script files and saves in UTF-8 (BOM, legacy tokens, binary save tokens),
   so that names from the culture lists and saves are UTF-8 too and memory holds one
   encoding.
3. **The original's own font atlases**: at start-up it replaces the game's 29 fonts by
   atlases it draws with DirectWrite (`load_font_atlas` at `0x15953C0`,
   `font_atlas_assets.cpp`), adds glyphs that are missing while the game runs into the
   reserved part of the texture (`native_font_atlas.cpp`, the texture synchronisation and
   the D3D9 hooks: Reset, CreateTexture, CreateVertexBuffer, SetTexture, DrawPrimitive,
   DrawIndexedPrimitive, SetStreamSource, with Reset chained after Anvil's overlay), tags
   glyph vertices with their page (the page blocks) and splits draws by page. The
   original writes its atlases into the game folder; here they belong in the plugin's data
   folder, which the game reads only once it is mounted (`PHYSFS_mount`, exported by
   `eu4.exe`). The texture limit (`texture_file_limit`) and the optional Source Han Sans /
   Plangothic pack (SIL OFL 1.1, not bundled) go with it. Then Chinese text needs no mod,
   and the text blocks can give a character above U+00FF its own index, as the original
   does, with its shaped-paragraph, line-break and kerning blocks.
4. **Editor and IME**: the original's input changes (about 30 blocks), UTF-8 typing and
   cursor movement, `sdl_ime_ui_less` for the candidate window, and how it works with
   Anvil's overlay, which also takes text input. After this the typed query is UTF-8 and
   the Windows-1252 conversion of `src/game_text.cpp` can go.

The original's code is built as C++17, as in its own build (`u8""` literals are `char`
there); the plugin itself is C++20.

## Build

```
git submodule update --init upstream
git -C upstream submodule update --init vendor/utfcpp
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Regenerate the symbols after moving `upstream/` to another commit:

```
python -I tools/gen-symbols.py "<EU4>/eu4.exe" upstream > symbols.toml
build/_deps/anvil_sdk-src/bin/anvil.exe check . --exe "<EU4>/eu4.exe"
```

`upstream/` is pinned to `50515b8` (v0.1.15 and one fix, 27.10.2026).
