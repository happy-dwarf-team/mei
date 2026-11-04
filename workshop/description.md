**Chinese, Japanese and Korean text in EU4, without the old DLL patch.**

Mei lets the game show, wrap, search and type the text of a Chinese, Japanese or Korean translation mod. It does not translate the game itself, so subscribe to a translation mod as well.

## What you get
- **Text that shows** in menus, tooltips, events and on the map. Translation packs made for the old double-byte patch work as they are.
- **Proper line breaks** between characters, and no more crash on a long paragraph.
- **Fonts included:** where the game's own fonts have no Chinese, Japanese or Korean letters, Mei brings fonts made from Source Han Sans, about 13,000 characters.
- **Search and typing:** the diplomacy country list and the province finder understand characters, pinyin and initials, and text you type or commit with an IME reaches text fields.

![](https://i.imgur.com/Uf8Z21n.png)

## How it works
EU4 keeps its text in an old Western encoding, so a Chinese, Japanese or Korean translation can't show without help. For years that help was a "double-byte" DLL patch (EU4DLL) dropped into the game folder. Mei does the same job as an [Anvil](https://anvil.happydwarf.dev/) plugin:

- **Encoding.** The game keeps its text in UTF-8 and draws it, in menus, tooltips, events and on the map. Translation packs made for the old patch (with its escape codes) are converted to UTF-8 as they load, so they work as they are. Script files and saves too: names that a supplementary pack keeps in scripts show as they read.
- **Line breaks.** Chinese and Japanese have no spaces, so the game had nowhere to break a line (and with a long paragraph it crashed). Mei breaks lines between characters, following Unicode's rules: never before a comma or a full stop.
- **Fonts.** A translation mod's own fonts are used as they are. Where the game would use its own fonts, which have no Chinese, Japanese or Korean letters, Mei brings fonts made from Source Han Sans (about 13,000 characters: Simplified and Traditional Chinese, Japanese kana and kanji, Korean Hangul).
- **Search.** The diplomacy country list and the province finder match Chinese characters, full pinyin, initials and mixed input, with one typo allowed in long pinyin.
- **Typing.** Text you type or commit with an IME reaches text fields as UTF-8, and Backspace, Delete and the arrow keys step by character. Selecting, undo (Ctrl+Z, Ctrl+Shift+Z) and paste work by character too.

![](https://i.imgur.com/Uf8Z21n.png)

## Mei does not translate the game
You need a translation mod as well. We tested Mei with these, each with Mei alone and the default settings:
- Simplified Chinese: [Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)
- Simplified Chinese names: [Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990), on top of the mod above
- Traditional Chinese: [Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)
- Korean: [Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)

We found no Japanese translation for EU4 1.37 on the Workshop; Mei draws Japanese text from any mod that has it.

![](https://i.imgur.com/Uf8Z21n.png)

## Before you subscribe
- **[Anvil](https://anvil.happydwarf.dev/) 1.9 or newer** must be installed in the game folder. Get it from [anvil.happydwarf.dev](https://anvil.happydwarf.dev/) and press Download. The Workshop can't install it for you.
- **EU4 1.37.5** (Steam, Windows).
- **Playing on GOG?** Mei also runs on the GOG build 1.37.5.2. GOG has no Workshop, so install Mei from the **Catalog** tab in Anvil Manager, or from the release zip on [GitHub](https://github.com/happy-dwarf-team/mei/releases).
- **Remove the old double-byte patch first** (`version.dll`, `d3d9.dll` and the `plugins` folder it put in the game folder). It and Anvil both use `version.dll`, so only one of them can be there.

![](https://i.imgur.com/Uf8Z21n.png)

## Getting it running
1. Subscribe to Mei and to your translation mod, and enable both in your playset.
2. Start the game. Anvil will tell you a new plugin is waiting.
3. Press **Ctrl+Shift+F12**, open **Mods** and approve Mei. You can also approve it before playing, in Anvil Manager.
4. Restart. Nothing to configure: the settings (**Ctrl+Shift+F12**, Settings) are already set for translation mods.

![](https://i.imgur.com/Uf8Z21n.png)

## Not there yet
- **IME candidate window:** typing through an IME was only checked with committed text; the candidate window and the text you are still composing were not checked.
- **Japanese kanji** in Mei's own fonts take their Simplified Chinese shapes (Source Han Sans SC).
- **Anvil's own panel and Manager** speak Chinese, Japanese and Korean from Anvil 1.9; with Anvil 1.8 they show Latin text only.

![](https://i.imgur.com/Uf8Z21n.png)

## Why "Mei"
銘 (*mei* in Japanese) is the signature a swordsmith cuts into the tang of a blade. The same character, 铭 (*míng*) in Chinese and 銘 (*myeong*, 명) in Korean, means "inscription": one character that all three languages share, as Mei serves all three. Our team names come from the forge.

![](https://i.imgur.com/Uf8Z21n.png)

## Credits and licences
Mei is a port of [EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch) by VulonLok (YozoraTempest), MIT licence: the text, search and input logic is theirs, built unchanged from their source. Thank you! Mei's own code is MIT too. Fonts: Source Han Sans by Adobe, SIL Open Font Licence 1.1. Not affiliated with Paradox Interactive.

![](https://i.imgur.com/Uf8Z21n.png)

## More from Happy Dwarf Team
- **[Anvil](https://anvil.happydwarf.dev/)**: the free loader that lets plugins like Mei into EU4, with a Manager to approve them and keep them updated.
- **[Bellows](https://steamcommunity.com/sharedfiles/filedetails/?id=3823747170)**: your game starts in about 13 seconds instead of 41.
- **[Hamweard](https://steamcommunity.com/sharedfiles/filedetails/?id=3832650792)**: Exit to Menu takes you back to the main menu without a restart.
- **[Tongs](https://steamcommunity.com/sharedfiles/filedetails/?id=3826695093)**: for mod testers and tool makers, let your scripts and AI agents see the game and press its buttons.
- **[Etch](https://etch.happydwarf.dev/)**: for interface modders, see your `.gui` window as the game draws it, right in VS Code.
- Everything we make: [happydwarf.dev](https://happydwarf.dev/)

![](https://i.imgur.com/Uf8Z21n.png)

### Alternative download
[GitHub](https://github.com/happy-dwarf-team/mei), also for the GOG build. The plugin page: [anvil.happydwarf.dev/plugins/mei](https://anvil.happydwarf.dev/plugins/mei/).
