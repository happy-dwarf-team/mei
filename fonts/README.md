# Mei's fonts

The game's fonts (`gfx/fonts/*.fnt`, 24 files in the Steam 1.37.5 build) have no glyph
above U+00FF. Mei brings five bitmap fonts with Chinese, Japanese and Korean glyphs and
puts them where the game would load one of its own fonts (`src/fonts.cpp`):

| Atlas | Size | Texture | For the game's fonts |
| --- | --- | --- | --- |
| `ui-14` | 14 px | 2048×2048 A8R8G8B8 | Arial12, Arial12_bold, garamond_12, garamond_14, garamond_14_bold, standard, standard_18, standard_22, tahoma_20_bold |
| `ui-16` | 16 px | 2048×2048 A8R8G8B8 | garamond_16, garamond_16_bold, vic_18, vic_18_grey, vic_18s |
| `ui-18` | 18 px | 2048×4096 A8R8G8B8 | vic_22, vic_22_bl, vic_22s |
| `ui-24` | 24 px | 2048×4096 A8R8G8B8 | garamond_24, vic_29, vic_29s, vic_36, vic_36s |
| `map` | 48 px | 4096×8192 BC3 with mips | Mapfont, tahoma_60 (map labels; the map shader takes the colour, so it is black) |

The sizes are the original's (upstream `src/font_assets.cpp`). Each atlas holds about
13,000 characters: all of GB 2312, level 1 of Big5, the Hangul syllables of KS X 1001
and level 1 of JIS X 0208 with their symbols and kana (`tools/mei-charset.py`), and
Latin-1 with the Windows-1252 characters at 0x80–0x9F, as the game's fonts hold them.
A character the font lacks (a few symbols) is left out and drawn as the game draws any
missing glyph.

A font that a mod supplies stays: an EU4DLL translation pack brings its own, and the
Korean packs that need no DLL draw Hangul from pairs of Latin-1 glyphs of their fonts.

## Source and licence

The glyphs are drawn from Source Han Sans SC 2.005R Regular by Adobe
([adobe-fonts/source-han-sans](https://github.com/adobe-fonts/source-han-sans)), under
the SIL Open Font License 1.1 (`SourceHanSans-OFL.txt`, shipped with the atlases as
`gfx/fonts/mei/OFL.txt`). The atlases are a converted form of that font, so they do not
carry its reserved name: their face is "Mei CJK". The font is the Simplified Chinese
release, so Japanese kanji and Traditional Chinese characters take its shapes.

The atlases are not in Git (about 140 MB). The release builds them from the pinned font,
checked by SHA-256, with the tools of this repository:

```
cmake --build build --config Release --target meifonts
pwsh tools/build-fonts.ps1 -MeiFonts build/Release/meifonts.exe -Out dist/fonts
```

`tools/meifonts.cpp` lays glyphs out as the original's `fontpack` does
(`src/font_atlas_assets.cpp`) and uses DirectWrite only with the given font file, never a
system font. Textures above 16 MiB load because Mei raises the game's texture file
limit to 64 MiB, as the original does (`texture_file_limit`).
