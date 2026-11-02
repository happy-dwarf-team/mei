# Unicode Patch for Anvil

An [Anvil](https://github.com/happy-dwarf-team/anvil) plugin for Europa Universalis IV
1.37.5 (Steam, Windows x64): a port of [EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch)
by VulonLok (MIT) from its EU4DLL-style `VERSION.dll` loader to Anvil, so that Chinese,
Japanese and Korean players no longer choose between their text patch and Anvil
([anvil#104](https://github.com/happy-dwarf-team/anvil/issues/104)).

**Status: draft.** It builds and its unit tests pass. The ported search finds the
province finder's English names, accented or not, as the game does. The localisation
import keeps the game's text in UTF-8, as the original does, but the game still draws it
as Windows-1252, so it stays off (`utf8_localization`) until the text drawing is ported.
`search` stays off by default until the diplomacy country list has been checked in the
game (see [State](#state)). Not affiliated with Paradox Interactive.

## Where it lives and why

Not a standard plugin in the Anvil repository. Anvil's design §4 keeps in the core only
what must work before any plugin loads or needs a view across all plugins, and keeps
game knowledge that only some plugins need (GUI, text) in those plugins. Standard plugins
are the few that every plugin may build on (`anvil-game`, `anvil-overlay`) or that every
player benefits from (`anvil-log-throttle`). This port is none of those:

- it changes how every text of the game is laid out, drawn and typed, which only players
  who read or write CJK (or other non-Latin) text need;
- it follows an active upstream (several releases a month) and is large: 287 symbols and
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

Two features of the original.

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

At the main menu the plugin logs how many values it kept, decoded and rejected. Until the
original's text drawing is ported, the game draws every byte of UTF-8 as a Windows-1252
character, so accented letters from the localisation show as two wrong ones ("Ã–stra
GÃ¶taland"); see [the in-game test](#in-game-test-of-the-localisation-import).

Settings (`settings.toml`, the overlay's panel): `search`, `utf8_localization`,
`typo_tolerance`, `fuzzy_pinyin`, which replace the original's `config.ini`. A `pinyin.txt` in the plugin
folder adds readings, as in the original.

`symbols.toml` declares every address of the original (91 blocks with their 113
continuations, 56 functions, 26 byte patches) and the game function the plugin calls
(`register_imported_text`), generated by `tools/gen-symbols.py` from the
original's hook tables, so that later features only add changes. `anvil check . --exe`
passes on the Steam `eu4.exe` (SHA-256 `9AD3EFE1…D771A`); `symbols.lock` is committed.

## State

| Step (plan in the EU4 engine lab, `docs/anvil-unicode-patch-research.md`) | State |
| --- | --- |
| Repository, licence, `plugin.toml`, `settings.toml`, CI workflow | written |
| `symbols.toml` with all 287 symbols | `anvil check --exe` OK |
| Search: 2 hooks, 19 byte patches | builds; `anvil check` of the built plugin folder OK |
| Localisation import: 2 hooks, 1 call | builds; off by default (`utf8_localization`) |
| The original's unit tests (UTF-8, EU4DLL escapes, search, pinyin, YAML), `tests/port_tests.cpp` and `tests/localisation_tests.cpp` | `ctest` 9/9 (Release, MSVC 2022) |
| In-game test of the province finder (2.11.2026, Anvil 1.6.0, Tongs 0.24.0, Russia 1719 save, English) | the regression found on 1.11 is fixed; see below |
| In-game test of the localisation import (2.11.2026, the same conditions) | 259096 values kept in UTF-8, none rejected; drawn wrong until the text drawing is ported; see below |
| In-game test of the diplomacy country list | not done |
| The rest of the original (script and save lexer, text drawing and layout blocks, fonts and atlases, map labels, editor and IME, dates, Rich Presence, save names) | not started; see [Next steps](#next-steps) |
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
break UTF-8 names. Both settings stay off until the text drawing is ported, and the import
should then switch the search on with it.

## Next steps

A plan only; none of it is ported. The original's order, from the lab's analysis:

1. **Script and save lexer** (`0x170CD10`, `0x170CA20`, `0x170D1D0`, `0x170E700`,
   `0x170EBF0`): script files and saves in UTF-8 (BOM, legacy tokens, binary save tokens),
   so that names from the culture lists and saves are UTF-8 too and memory holds one
   encoding.
2. **Text drawing and layout**: the blocks that walk text byte by byte (width, wrapping,
   cursor, popups, map labels) read UTF-8 code points. After this, `utf8_localization` can
   default to on together with `search`, and `src/game_text.cpp` only handles the typed
   query.
3. **CJK fonts**: the font loader's code point limit (`font_codepoint_limit`, `0xFF` →
   `0x10FFFF`) and texture size limit (`texture_file_limit`), the original's atlas
   functions and its D3D9 hooks, glyphs from the optional Source Han Sans / Plangothic pack
   (SIL OFL 1.1, not bundled), and where the atlas cache lives (the plugin's data folder,
   not the game folder).
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
