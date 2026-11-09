# How the port works

For developers. What Mei gives a player is in the [README](../README.md).

## Where it lives and why

Not a standard plugin in the Anvil repository. Anvil's design §4 keeps in the core only
what must work before any plugin loads or needs a view across all plugins, and keeps
game knowledge that only some plugins need (GUI, text) in those plugins. Standard plugins
are the few that every plugin may build on (`anvil-game`, `anvil-overlay`) or that every
player benefits from (`anvil-log-throttle`). This port is none of those:

- it changes how every text of the game is laid out, drawn and typed, which only players
  who read or write CJK (or other non-Latin) text need;
- it follows an active upstream (several releases a month) and is large: 315 symbols and
  about 170 changes;
- its licence and credit belong to the original's author.

So it is a plugin of its own, in this repository, which builds the original's text logic
from the pinned `upstream/` submodule instead of copying it. Upstream's own repository
(an Anvil target next to its proxy) stays the preferred long-term home; that needs the
author's agreement.

## What it does so far

Eight features of the original.

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

### Localisation import (`utf8_localization`)

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

### Script and save lexer (`utf8_scripts`)

The game reads scripts and saves byte by byte, so UTF-8 passes through its lexer as it is,
except for a signature, which it takes as the start of the first token. EU4DLL translation
packs keep their names in scripts as escapes, and old saves hold them too, so a
supplementary translation mod showed garbage in place of rulers, capitals and provinces
([the test](../README.md#in-game-test-of-the-script-and-save-lexer)). As in the original (`src/script_lexer.cpp`):

| Symbol | The game | The plugin |
| --- | --- | --- |
| `construct_script_file` `0x170CD10`, `construct_script_file_mode` `0x170CA20`, `construct_script_stream` `0x170D1D0` | opens a script | a UTF-8 signature at the start is skipped |
| `read_legacy_script_token` `0x170E700` | reads one token of a script or a text save | a text token with EU4DLL escapes becomes UTF-8 in the token's buffer of 512 bytes; other tokens, and Windows-1252 names, stay as they are |
| `read_legacy_binary_token` `0x170EBF0` | reads one token of a binary save | the same for strings |

A byte scan comes before the original's import, as the game reads millions of tokens and
few have an escape. As in the original, a token that cannot be decoded ends the file or
save for the game, and the log says which (none in the tests). At the main menu and at each
session the plugin logs how many tokens it decoded.

**Ironman, multiplayer and saves.** Only the text of names changes: two new games with the
same seed, with and without `utf8_scripts`, gave saves whose 1.6 million lines differ in
names alone ([the test](../README.md#in-game-test-of-the-script-and-save-lexer)), with the same count of random numbers. Files are not touched, so the
game's checksum stays. So `utf8_scripts` stays on in ironman with the rest of the plugin
(`affects_gameplay = false`). A save written with it holds these names in UTF-8, which a
game without Mei shows as Latin-1 letters. In multiplayer every player needs the same mods
anyway; a player without Mei sees such names wrongly. Whether names can ever feed the
game's checks for desynchronised games is not checked (no multiplayer session was run).

### Text drawing (with `utf8_localization`)

The game walks text byte by byte and takes each byte as the index into its font's 256
glyphs, so without these blocks UTF-8 accents show as two wrong letters ("Ã–stra
GÃ¶taland", [the test before them](../README.md#in-game-test-of-the-localisation-import)). 36 of the
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
sees after a block (kerning) is always that byte, never above 0xFF, so the original's
blocks for its own glyphs (shaped paragraphs, atlas pages and kerning above U+00FF) are
not needed. Text that is not UTF-8, such as Windows-1252 names from scripts and saves,
stays one byte per glyph, so both encodings draw side by side. The decoder is the
plugin's own: the original's throws a C++ exception on every byte that is not UTF-8,
which here would be every accented name of a save in every frame.

The game ends a line only after a glyph whose height field is 0, a space, and moves the
glyphs after it to the next line, so a Chinese or Japanese paragraph has nowhere to break;
a long one (the 1444 bookmark's description in a Chinese translation) crashed the game in
the lobby, with or without this plugin
([the test](../README.md#in-game-test-with-translation-mods)). Three more blocks of the original
(`main_wrap_hook`, `button_wrap_hook`, `popup_wrap_hook`) let a line also end after any
glyph where Unicode's line breaking rules allow a break after it (UAX #14 through the
original's `formatted_text.cpp`, so not before a comma, a full stop or a closing bracket,
and not after an opening one). The original's main and button blocks ask the rules about
the break before a character above U+00FF, while the game puts the break after it, so a
line could start with ，or 」 and end with 「; Mei asks about the break after the glyph in
all three. Text without a character of three or more bytes keeps the game's breaks and
costs nothing.

The game strips colour markers (`CString::RemoveTextColoring`, `construct_colorless_text`
`0x17046A0`) byte by byte: it drops a 0xA7 byte and the byte after it. In UTF-8 that is the
last byte of a character such as 大 or 控 and the first byte of the next one, and of a
marker `§Y` it leaves the 0xC2, so a stripped name showed stray letters or broken text
(mercenary company names, institution tooltips, the game set-up's names, `/save`). As in
the original (v0.1.16, `native_color_strip.cpp`), the plugin replaces the function: whole
markers go, every other byte stays, single-byte markers of the game's own text too.

### CJK fonts

The game's font loader drops every glyph above U+00FF (`cmp edi, 0xFF`), and the Steam
1.37.5 fonts have none. As in the original, the limit becomes U+10FFFF together with the
loader's lookup, allocation and store blocks (`font_hooks.asm`) and the original's glyph
registry (`glyph_registry.cpp`), which keeps those glyphs apart from the font's table of
256 (alone, the limit would let the loader write past it); `destroy_font_table` releases
them. So a font that holds Chinese, Japanese or Korean glyphs on its one texture page
draws them with the game's own renderer. The texture file limit goes from 16 to 64 MiB,
as in the original (`texture_file_limit`): EU4DLL translation packs and Mei's own fonts
have larger textures.

Translation mods made for EU4DLL bring such fonts and keep them. Where the game would load
one of its own fonts instead, Mei's mod has an atlas of the same size from Source Han
Sans (about 13,000 characters; [`fonts/README.md`](../fonts/README.md)), and the hook on the
font loader's entry (`load_font_atlas`, `src/fonts.cpp`) puts its path in place, as the
original does with its own atlases. A font that a mod supplies is never replaced, also
without CJK glyphs: Korean packs that need no DLL draw Hangul from pairs of Latin-1 glyphs
of their own fonts (`bundled_fonts`, on by default).

The original instead draws the game's fonts at start-up from the system's fonts and adds
missing glyphs while the game runs (texture uploads and D3D9 hooks); that is not ported.
Its `fontpack` tool (built here) and `tools/make-font-mod.py` still make a test mod's
fonts from the system's fonts.

### Map labels (with `utf8_localization`)

Country names on the map come from the localisation and go through their own routines.
17 of the original's map blocks (`map_hooks.asm`) decode them as the text blocks do:

| Game routine | Blocks |
| --- | --- |
| province labels `0x159DA60` | copy, measure, draw |
| the label fit `0x159E510` | format, measure, icon end |
| country labels along their curve `0xFD3850`, `0xFD5B70` | draw, character count (the letter spacing divides by characters, not bytes), spacing, advance |
| letter spacing `0xFD7200` | copy, glyph, gap end, last character, vertex count |
| case change of label text `0x14BA7F0`, `0x15503F0` | upper and lower case of ASCII letters only, as in the original (the C runtime's byte-wise change would alter UTF-8 bytes) |

As for the interface, the original's whole-label shaping, atlas page tags and kerning for
characters above U+00FF are not ported. Mei's map atlas (for Mapfont and tahoma_60) is
black, as the map shader takes the atlas colour.

### Text input (`utf8_input`)

SDL hands the game each typed or IME-committed text as UTF-8; the game keeps only what its
conversion allows, so Chinese, Japanese and Korean never reach a text field. As in the
original (`src/text_input.cpp`):

| Symbol | The game | The plugin |
| --- | --- | --- |
| `input_block` `0x1569F91` | per-byte handling of SDL's text event | the original's `dispatch_utf8`: the whole text as one tagged event on the game's queue; continues at `input_return` |
| `consume_editor_commit` `0x15354E0` | a text field takes one character | for the queue's caller only: the whole tagged text, through the field's refused characters and the game's seven-character restriction |
| `insert_editor_commit` `0x1536B80` | inserts the field's text | for that consumer's call only: inserts the whole text |
| `editor_key` `0x15366C0`, `editor_left` `0x15384D0`, `editor_right` `0x15385A0` | Backspace, Delete and the arrows step by byte | in a field whose text is UTF-8, they step by grapheme; Ctrl+A selects all, Ctrl+Z and Ctrl+Shift+Z undo and redo, up and down move between rows (the original's `editor_key` and `editor_arrow` without its shaped paragraphs and IME state; `native_editor_text.cpp`) |
| `editor_selection` `0x153B170` | keeps a selection in bytes | takes in every grapheme it touches, whichever way it runs |
| `editor_point` `0x15361F0` | a click lands on a byte | on the nearest grapheme boundary |
| `paste_editor_clipboard` `0x1539240` | pastes what Windows-1252 has | the clipboard's UTF-8 as one commit, through the field's refused characters |
| `filter_editor_text` `0xB19590` | takes refused Latin-1 bytes out of the text, also out of UTF-8 sequences | for the field's two calls, takes out whole characters |
| `editor_fit_block` `0x1536E51` | drops a text's last byte when it does not fit | drops its last grapheme |
| `editor_width_fit` `0x1537210`, `editor_word_break` `0x1539820`, `construct_editor_space_row` `0x1539620` | fit and wrap a row by bytes | in whole graphemes |
| `prefix_assign_block_*` (three calls of `std::string` assign) | cut a text at a byte limit | at the last whole grapheme. The original hooks the assign itself, which has 17,000 call sites; Mei replaces the three calls |
| `destroy_editor` `0x1533D90` | destroys a field | forgets its undo history |
| `focus_editor_ime_rect` `0x1535250`, `blur_editor_ime_rect` `0x15353F0`, `hide_editor` `0x15340A0`, `show_native_ime_candidates` `0x1764940` | focus, and SDL's window messages | the game's dispatch consumes Ctrl+Shift+Z before a field sees it, so Ctrl+Z with or without Shift is taken from the window's messages while a field has the keyboard (the original's path, without its IME state) |

`insert_editor_commit` also records the undo step and puts the caret on a grapheme boundary
after a commit, as in the original. Fields with text that is not UTF-8 (a Windows-1252 name)
keep the game's handling. The temporary editors of tooltips and the lobby's chat (two more
call sites) are left alone, as in the original. There is no word step on the keyboard to
port: the game's key handler has none, and word breaks are the soft wraps above.

As in the original, SDL's IME set-up skips the UI-less TSF manager (`sdl_ime_ui_less`),
which would ask the game to draw the candidates itself, so the IME's own candidate window
shows (not checked with a real IME).

Not ported yet: the composition display, how it works with Anvil's overlay, which takes
text input too, and the original's selection boxes, which only shaped paragraphs and fields
of several rows need.

### Steam Rich Presence (with `utf8_localization`)

The game converts the Rich Presence text it hands Steam (the country played, say) from
Windows-1252 to UTF-8, so a name that the localisation import keeps in UTF-8 reached Steam
friends converted twice, as Latin-1 letters. The original hooks the conversion and changes
only the call whose return address is the Steam one (`native_steam_presence.cpp`); Mei
redirects that call alone:

| Symbol | The game | The plugin |
| --- | --- | --- |
| `presence_conversion_call_a901fe` | calls `convert_steam_presence` `0x1706010` on the Rich Presence text | the original's `construct_steam_presence`: valid UTF-8 is assigned as it is (through `assign_editor_prefix`, the game's `std::string` assign), anything else goes through the game's conversion |

The game's other caller of the conversion keeps it. The GOG builds have no Steam Rich
Presence: the call and the function are declared absent there.

`symbols.toml` declares every address of the original (91 blocks with their 113
continuations, 57 functions, 29 byte patches), the call site of the Rich Presence text, the game functions the plugin calls
(`register_imported_text`, `repeat_text`, `append_text`), the copy buffer of the main
text routine and the game's operator new (values of their blocks' bytes), generated by `tools/gen-symbols.py` from the
original's hook tables, so that later features only add changes. `anvil check . --exe`
passes on the Steam `eu4.exe` (SHA-256 `9AD3EFE1…D771A`); `symbols.lock` is committed.

## Next steps

A plan only; none of it is ported. The original's order, from the lab's analysis:

1. **The lower-case block's cost**: 46 million calls in a short session. Lowering the
   string in one callback for the whole loop, instead of a block per byte, would cut the
   calls by the string's length; it needs the loop's exit continuation.
2. **The original's own font atlases** (in place of Mei's fixed ones): at start-up it
   draws its atlases with DirectWrite from the system's fonts (`font_atlas_assets.cpp`),
   adds glyphs that are missing while the game runs into the
   reserved part of the texture (`native_font_atlas.cpp`, the texture synchronisation and
   the D3D9 hooks: Reset, CreateTexture, CreateVertexBuffer, SetTexture, DrawPrimitive,
   DrawIndexedPrimitive, SetStreamSource, with Reset chained after Anvil's overlay), tags
   glyph vertices with their page (the page blocks) and splits draws by page. The
   original writes its atlases into the game folder; here they belong in the plugin's data
   folder, which the game reads only once it is mounted (`PHYSFS_mount`, exported by
   `eu4.exe`). Then every character has a glyph, in the shapes of the player's own
   script, and the text blocks can give a character above U+00FF its own index, as the
   original does, with its shaped-paragraph and kerning blocks.
3. **The IME**: the composition display and how it works with Anvil's overlay, which also
   takes text input; then a test with a real IME (Microsoft Pinyin, Microsoft IME for Japanese and Korean). After this the typed query
   is always UTF-8 and the Windows-1252 conversion of `src/game_text.cpp` can go.

The original's code is built as C++17, as in its own build (`u8""` literals are `char`
there); the plugin itself is C++20.
