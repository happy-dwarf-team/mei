# Mei 銘

**Chinese, Japanese and Korean text in Europa Universalis IV, without the old DLL patch.**
Mei does not translate the game. It lets the game show, wrap, search and type the text of a
translation mod.

Mei is an [Anvil](https://anvil.happydwarf.dev/) plugin for EU4 1.37.5 (Steam, and the GOG
build 1.37.5.2, Windows x64). It is a port of
[EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch) by VulonLok (MIT), so
that Chinese, Japanese and Korean players no longer choose between their text patch and Anvil
([anvil#104](https://github.com/happy-dwarf-team/anvil/issues/104)). Not affiliated with
Paradox Interactive.

銘 (*mei* in Japanese) is the signature a swordsmith cuts into the tang of a blade. The same
character, 铭 (*míng*) in Chinese and 銘 (*myeong*) in Korean, means "inscription", one
character that all three languages share. The team's names come from the forge.

**Status: works with translation mods.** Tested in the game with the most used Simplified
Chinese, Traditional Chinese and Korean translations from the Workshop ([State](#state)). What
was not tested is under [Limitations](#limitations).

## What you get

With the default settings:

- **Text shows.** The game keeps a translation mod's text in UTF-8 instead of turning it into
  `?`, and decodes the escapes of EU4DLL translation packs.
- **Names show too.** Names in scripts and saves work the same way, so rulers, capitals and
  provinces of a supplementary translation mod read as they should.
- **Lines wrap.** Chinese and Japanese paragraphs break where Unicode's rules allow.
- **Fonts.** A translation mod's fonts stay. Where the game would use its own, which have no
  such letters, it uses [Mei's fonts](fonts/README.md), from Source Han Sans (about 13,000
  characters).
- **Map labels.** Country names on the map read right, spaced by character.
- **Search.** The diplomacy country list and the province finder match Chinese characters, full
  pinyin, initials and mixed input, with one typo allowed in long pinyin.
- **Typing.** Typed and IME-committed text reaches text fields. Keys, the mouse, selection, undo,
  copy and paste work by character.

## Install

- [Anvil](https://anvil.happydwarf.dev/) 1.9 or newer in the game folder, and EU4 1.37.5
  (Steam, or the GOG build 1.37.5.2 with the GitHub release and a local mod for the fonts).
- Remove an old double-byte patch first (`version.dll`, `d3d9.dll` and the `plugins` folder of
  EU4DLL or the original). It and Anvil both use `version.dll`.
- **[Steam Workshop](https://steamcommunity.com/sharedfiles/filedetails/?id=3832685486):** subscribe to Mei and a translation mod, enable both in the playset,
  approve Mei in Anvil (Ctrl+Shift+F12, Mods) and restart. The Workshop item carries the
  plugin (`anvil/mei/`) and Mei's fonts (`gfx/fonts/mei/`).
- **GitHub release:** `mei-windows-x64.zip` holds the plugin folder `mei/` for
  `anvil/plugins/`; `mei-fonts.zip` holds the fonts, which the game reads from a mod's
  `gfx/fonts/mei/` only (a local mod with them, or the Workshop item). Without them the
  game's own fonts stay and a translation mod's fonts still work.


## Settings

Nothing needs configuring for a translation mod. To change a setting, use Mei's panel in the
Anvil overlay or `anvil/data/mei/settings.toml`. Every setting applies at the next start.

| Setting | Default | What it does |
| --- | --- | --- |
| `search` | on | Chinese and pinyin search. Keep it on with `utf8_localization`: the game's own search misses accented names in UTF-8 |
| `utf8_localization` | on | The game keeps its localisation in UTF-8 and draws it. The text blocks, the font loader and the map labels come with it |
| `utf8_scripts` | on | Script files and saves may be in UTF-8, and EU4DLL escapes in them become UTF-8. Needs `utf8_localization`. Saves written with it keep those names in UTF-8 |
| `bundled_fonts` | on | Uses Mei's fonts where the game would use its own. Needs `utf8_localization` |
| `utf8_input` | on | Typed and IME-committed text reaches text fields as UTF-8 |
| `typo_tolerance` | on | Full pinyin of 5 or more letters still matches with one letter wrong, missing, added or two neighbours swapped |
| `fuzzy_pinyin` | off | zh/z, ch/c, sh/s, n/l, en/eng and in/ing match each other |

A `pinyin.txt` in the plugin folder adds readings, as in the original.

## Limitations

- **Saves.** A save written with `utf8_scripts` holds these names in UTF-8, which a game without
  Mei shows as Latin-1 letters. Files are not touched, so the game's checksum stays.
- **Multiplayer: not tested.** No multiplayer session was run. Every player needs the same mods
  anyway, and a player without Mei sees such names wrongly. Whether names can feed the game's
  checks for desynchronised games is not checked.
- **Ironman.** Mei stays on in ironman (`affects_gameplay = false`). In the tests, only the text of
  names changed between two games with the same seed.
- **Fonts.** A character that no font has shows as `?`. The original's atlases, drawn from the
  system's fonts while the game runs, are not ported.
- **IME.** The composition display is not ported, and typing with a real IME is not tested: the
  tests sent finished text to the window.
- **Not tested:** a Japanese translation mod (there is none for 1.37 on the Workshop, though the
  probe text in Japanese drew right), the search in the diplomacy country list, a text field of
  several rows, selecting with a mouse drag, and long play.
- **Not ported:** the original's dates, Rich Presence and save names. Name order, which changes
  generated characters, would be a separate plugin.

## State

| Step (plan in the EU4 engine lab, `docs/anvil-unicode-patch-research.md`) | State |
| --- | --- |
| Repository, licence, `plugin.toml`, `settings.toml`, CI workflow | written |
| `symbols.toml` with all 311 symbols | `anvil check --exe` OK |
| Search: 2 hooks, 19 byte patches | on (`search`) |
| Localisation import: 2 hooks, 1 call | on (`utf8_localization`) |
| Script and save lexer: 5 hooks | on with `utf8_localization` (`utf8_scripts`) |
| Text drawing: 36 blocks and 3 line break blocks | on with `utf8_localization` |
| Font loader: the U+10FFFF limit, the 64 MiB texture limit, 3 blocks, 1 hook, the glyph registry | on with `utf8_localization` |
| Mei's fonts: the font loader's entry, 5 atlases (`tools/meifonts.cpp`, `tools/build-fonts.ps1`) | on (`bundled_fonts`) |
| Map labels: 17 blocks | on with `utf8_localization` |
| Text input: 5 blocks, 17 hooks, the IME patch | on (`utf8_input`) |
| The original's unit tests (UTF-8, EU4DLL escapes, script signature, editor, search, pinyin, YAML), `tests/port_tests.cpp`, `tests/localisation_tests.cpp` and `tests/text_drawing_tests.cpp` | `ctest` 13/13 (Release, MSVC 2022) |
| In-game test with translation mods (3.11.2026, Anvil 1.8.0, Tongs 0.25.1) | Simplified and Traditional Chinese and Korean shown, wrapped and searched with the defaults; see below |
| In-game test of the script and save lexer (4.11.2026, Anvil 1.9.0, Tongs 0.26.0, Steam and GOG) | names of the supplementary Chinese mod shown; state unchanged but for names; see below |
| In-game test of the province finder (2.11.2026, Anvil 1.6.0, Tongs 0.24.0, Russia 1719 save, English) | the regression found on 1.11 is fixed; see below |
| In-game test of the localisation import (2.11.2026, the same conditions) | 259096 values kept in UTF-8, none rejected; see below |
| In-game test of the text drawing (2.11.2026, the same conditions) | accented localisation drawn right; no measurable cost per frame; see below |
| In-game test of CJK fonts from a mod (2.11.2026) | Chinese, Japanese and Korean drawn right in the UI and, with the map blocks, on the map; see below |
| In-game test of the text field (4.11.2026, Steam and GOG) | selection, undo, redo, paste, cut and copy by character; a field of several rows and a mouse drag not checked; see below |
| In-game test of the text input (2.11.2026) | typed Chinese, Japanese and accented text reaches the field whole and is found; IME composition not checked; see below |
| In-game test of the diplomacy country list | not done |
| The rest of the original (its own font atlases, the IME composition display, dates, Rich Presence, save names) | not started; see [Next steps](docs/port.md#next-steps) |
| Name order (a separate plugin with `affects_gameplay = true`) and the mod registry fix | not started; the mod registry fix decides which mods load and does not belong here |

### In-game test with translation mods

On 2–3.11.2026 with Anvil 1.8.0, Tongs 0.25.1 and this plugin with its default settings,
in a junction copy of the game with a private game profile (`-userdir`) that enables one
translation mod from the Workshop and a local mod with Mei's five atlases
(`gfx/fonts/mei/`); the game at 3440×1440, 145–166 frames per second in every run. Each
start went to the main menu and the single-player lobby; the Simplified Chinese one on
into a game (the Ottomans, 1444–1446 at speed 5).

| Translation mod | What it is | Result |
| --- | --- | --- |
| [Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733) (Simplified Chinese, about 340,000 subscribers) | EU4DLL escapes, its own fonts (`zh-hans-*`, 21,563 glyphs) | 248,798 values decoded from escapes, none rejected; menu, lobby, map labels, the bookmark text, the 1444 introduction and messages in Chinese, wrapped; `falanxi` finds 法兰西 and 法兰西岛 |
| [Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432) | EU4DLL escapes, its own fonts (`Trad-zh-*`, textures up to 32 MiB) | 248,804 values decoded; menu, lobby, map and bookmark text in Traditional Chinese, wrapped; `yinggelan` finds 英格蘭, 新英格蘭海山 and 北英格蘭 |
| [Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430) | no DLL: Hangul drawn from pairs of Latin-1 glyphs of its own fonts | its fonts kept (45 font paths); menu and lobby in Korean as without the plugin, map labels in English as the mod intends |

- Mei's fonts replaced only the game's own: 0 font paths with the Simplified Chinese mod
  (it maps every game font to its own), 2 with the Traditional Chinese and the Korean one.
  Without a translation mod, with the original's probe localisation, they replaced all 47
  and drew the menu, the lobby and the map labels in Chinese, Japanese and Korean.
- Before the line break blocks, the Simplified Chinese mod crashed the game when the lobby
  opened: an access violation in `EOSSDK-Win64-Shipping.dll` on the main thread, with UTF-8
  text of the bookmark's description in a register, in three of three starts. It crashed
  the same way with the localisation import off (the register then held the mod's EU4DLL
  escapes) and with this plugin disabled, so it is the game's own failure on a paragraph
  without spaces; the mod's page warns that it crashes without its DLL. With the line break
  blocks the lobby opened in every start.
- Not checked: Japanese (no Japanese translation for 1.37 on the Workshop; the probe
  localisation's Japanese drew right), an IME's candidate window, and long play.

### In-game test of the script and save lexer

On 4.11.2026 with Anvil 1.9.0, Tongs 0.26.0 and this plugin's default settings, in junction
copies of the Steam 1.37.5 and the GOG 1.37.5.2 game with a private game profile: the
Simplified Chinese mod, its [supplementary mod](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)
(EU4DLL escapes in 5,763 script files) and a probe mod with one script file in UTF-8 with a
signature, which renames two provinces. New games at 1444 with a fixed seed (`-seed`).

- **Scripts:** 229,719 script tokens decoded, none rejected, the same on both builds. Rulers,
  capitals and province names show in Chinese; with `utf8_scripts` off the ruler's name in
  the lobby shows Latin-1 letters.
- **Signature:** the probe's province 1 reads 梅迪 with `utf8_scripts`; without it the game
  drops the first entry and the province keeps its name.
- **Saves:** a text save holds the names in UTF-8, with no escape left; with `utf8_scripts`
  off the same game's save has 152,536 escape bytes. A binary ironman save written with it off
  (346,592 escape bytes) loads with it on: 29,737 tokens decoded, none rejected, in ironman
  (Mei stayed on, Tongs was suspended by Anvil as it should be).
- **State:** the saves of the two seeded games, one with `utf8_scripts` and one without, are
  1.67 million lines. After decoding the escapes, 142 lines differ: names, capitals, the file's
  own hash, and the probe's province. `multiplayer_random_count` is 163,153 in both.
- No multiplayer session was run.

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
- Before the map blocks, the map label of the same country showed Latin-1 letters in
  place of 奥斯曼. With them (and the map atlas for Mapfont) the 1444 map in the lobby reads
  奥 斯 曼 along the Ottoman curve, spaced by characters, and 英格兰, 卡斯蒂利亚 and 法兰西 for
  England, Castile and France; the other labels are in English as before.
- Map label cost on the lobby's map (165 frames per second): main-thread CPU per frame
  1.49 ms with the map blocks, 1.58 ms with the build before them. In that run (about
  12,000 frames, the lobby open most of it) the map blocks ran about 2.6 million times and
  the lower-case block 46.2 million times: `0x15503F0` lowers strings byte by byte for more
  than the map (searches, string comparisons), so its block is the busiest of the plugin.
- Clicking the lobby's Back ended that game: it exited by itself through `ExitProcess` (Anvil
  wrote its call counts), with no crash report. The build before the map blocks does the
  same in this profile, so it is not the map blocks; why the game exits there is not known.
- Without the plugin's changes the menu is in English: the game leaves out the probe file,
  which has no UTF-8 signature (the localisation import gives it one).
- Cost at the main menu (165 frames per second): the text blocks ran about 0.2 times a frame
  there (the game keeps laid-out menu text), the font loader blocks 15,812 times while the
  fonts loaded. Main-thread CPU per frame 1.14 ms with the plugin, 0.97 and about 1.04 ms
  without: no difference that the blocks could explain at that call rate.
- Starting the private profile straight into the save (`continue=`) crashed twice in
  `pops_api.dll` (a null read, no plugin on the stack); starting it in the menu did not.
  The first start ran with the display asleep (8 frames per second) and does not count.

### In-game test of the text input

Three starts on 2.11.2026 in the same private profile and test mod (165 frames per second),
in the single-player lobby's finder (Find, the same `goto_box` as the province finder).
Text was sent as `WM_CHAR` messages to the game window (`PostMessageW`, no change of
focus), which SDL turns into the same text event an IME commit ends in; Tongs typed Latin-1.

- With `utf8_input`: 法兰 reached the field whole as UTF-8, and the finder listed 法兰西
  (France) by the Chinese search; 法xö日本 (Latin, accented and Japanese mixed) too.
- Without it the same messages left only xö in the field, in Windows-1252: the game drops
  Chinese and Japanese.
- Before the key hooks one Backspace removed one byte of 兰 (the field showed 法 and a
  broken letter). With them: Backspace removed 西 whole, Left then x gave 法x兰, Delete
  removed 兰 whole, Tongs' bä then Backspace left b, and a Latin query (bohemia) found
  Bohemia.
- Not checked: IME composition itself (the candidate window, `WM_IME_COMPOSITION` and the
  composing text in the field). A real IME commit reaches the game through
  `ImmGetCompositionString`, which a message from another process cannot fill, so it needs
  typing with an IME by hand.

### In-game test of the text field

On 4.11.2026 with Anvil 1.9.0, Tongs 0.26.0 and the Simplified Chinese mods, on the Steam
and the GOG build, in the lobby's finder. Text went to the window as `WM_CHAR`, keys through
Tongs, and Ctrl and Shift as window messages with the game thread's key state set, as Tongs
has no modifiers; the lab's clipboard was put back after each run. Both builds gave:

| Step | The field |
| --- | --- |
| 法兰西, Ctrl+A, Backspace | empty |
| 法兰西abc, Ctrl+Z twice, Ctrl+Shift+Z | 法兰西ab, 法兰西a, 法兰西ab |
| 法兰西abc, Shift+Left four times, Ctrl+C | the clipboard holds 西abc |
| Backspace, Ctrl+Z | 法兰, then 法兰西abc |
| Ctrl+V with 日本語テスト, Ctrl+Z | 法兰西abc日本語テスト, then 法兰西abc |
| Left four times, x | 法兰x西abc |
| Ctrl+A, Ctrl+V with 한국어 | 한국어 |
| Ctrl+A, Ctrl+X | empty, and the clipboard holds 한국어 |
| e with a combining accent, x, Backspace twice | empty |
| 法兰西abc, a click near the field's left, y | 法兰y西abc |
| Ctrl+V with 240 Chinese characters | 24 characters that fit the field, valid UTF-8 |

Tongs' `bä`, Backspace (b), `falanxi` (the finder's list) and the tooltip of a lobby shield,
which goes through a temporary editor, work as before.

- Before the window message hook, Ctrl+Shift+Z never reached the field: the log showed its
  Shift and Ctrl, and no Z.
- Not checked: a field of several rows (none found in the game's interface files that a player
  reaches easily), selecting with a mouse drag (window messages for it did not reach the
  game, the hooks for clicks and selections ran by other paths), and refused characters by
  hand (the original's tests cover them).

### Cost of the lexer and the text field

On 4.11.2026 on the Steam build, Anvil 1.9.0 and Tongs 0.26.0, in a new game's profile with the
files in the disk cache, 3440×1440.

| Measure | Without | With |
| --- | --- | --- |
| Start to menu with both Chinese mods (`game ready at`, median of 3, `utf8_scripts` off and on) | 31.95 s (31.3–35.0) | 32.81 s (31.3–34.4) |
| Start to menu without mods, the same arms | 25.83 s (25.7–26.9) | 25.93 s (25.7–29.2) |
| Main-thread CPU per frame on the paused 1444 map, the 0.2.0 release and this build (6 samples of 30 s in 2 starts each) | 4.31 ms (4.06–4.56) | 4.28 ms (4.09–4.77) |

The differences are smaller than the spread between starts of one arm. The lexer's hooks run
while scripts and saves load, and the text field's when a field is used or a tooltip is built,
so nothing was expected per frame.

With Bellows 0.19.0 next to Mei (both Chinese mods) the game started in 14.7 s, 5 plugins
loaded, the same 229,719 tokens were decoded and the save holds the same names (one start).

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

How the port works, symbol by symbol, why it is a plugin of its own and what could come next:
[docs/port.md](docs/port.md).

## Steam Workshop

[`workshop.toml`](workshop.toml) publishes `workshop/` with
[paradox-workshop-publish](https://github.com/happy-dwarf-team/paradox-workshop-publish):
the descriptor, the thumbnail, the plugin folder and the fonts of a release, which
`workshop-publish.yml` takes from the published GitHub release after checking their
attestation. The descriptions are `workshop/description.md` (English),
`description.schinese.md` and `description.tchinese.md`, with the Chinese titles in
`workshop.toml`. Steam takes the Chinese titles and descriptions only from client mode
(`pdx-ws publish --mode client` on a PC with Steam signed in as the item's owner); CI's
steamcmd publish keeps them as they are.

## Licence

MIT (`LICENSE`), the original's copyright notice and this port's. The original is MIT
© 2026 VulonLok, which allows this port with the notice kept. Its parts that the port
builds: utfcpp (BSL-1.0) and phrase-pinyin-data (MIT), both through `upstream/`.
MinHook, the MASM stubs and the proxy loader are not used.
