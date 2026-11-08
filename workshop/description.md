![](https://anvil.happydwarf.dev/workshop/mei-hero.png)

Mei lets the game show, wrap, search and type the text of a Chinese, Japanese or Korean translation mod. It does not translate the game itself, so subscribe to a translation mod as well.

![](https://anvil.happydwarf.dev/workshop/mei-install.png)

![](https://anvil.happydwarf.dev/workshop/mei-steps.png)

**Remove the old double-byte patch first** (`version.dll`, `d3d9.dll` and the `plugins` folder it put in the game folder). It and Anvil both use `version.dll`, so only one of them can be there.

Needs [Anvil](https://anvil.happydwarf.dev/) 1.9 or newer and EU4 1.37.5 (Steam; the GOG build 1.37.5.2 too, see the FAQ). If Windows warns about Anvil, choose **More info**, then **Run anyway**.

![](https://anvil.happydwarf.dev/workshop/mei-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-extra0.png)

You need a translation mod as well. We tested Mei with these, each with Mei alone and the default settings:
- Simplified Chinese: [Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)
- Simplified Chinese names: [Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990), on top of the mod above
- Traditional Chinese: [Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)
- Korean: [Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)

We found no Japanese translation for EU4 1.37 on the Workshop. Mei drew our Japanese test text, but no full translation was tried.

![](https://anvil.happydwarf.dev/workshop/mei-extra1.png)

- **IME:** no real IME was used. The tests sent finished text to the window, so the candidate window and the text you are still composing were not checked.
- **Japanese kanji** in Mei's own fonts take their Simplified Chinese shapes (Source Han Sans SC).
- **Anvil's own panel and Manager** speak Chinese, Japanese and Korean from Anvil 1.9; with Anvil 1.8 they show Latin text only.

[![](https://anvil.happydwarf.dev/workshop/mei-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-github.png)](https://github.com/happy-dwarf-team/mei)

[![](https://anvil.happydwarf.dev/workshop/discord-en.png)](https://discord.happydwarf.dev/)

![](https://anvil.happydwarf.dev/workshop/mei-faq.png)

### Why do I have to approve it?
Mei is a real program running inside your game, not just a pile of text files. Anvil makes sure nothing like that runs without your nod. Trust, but verify. Dwarf law.

### I play on GOG. Can I use it?
Yes, on the GOG build 1.37.5.2. GOG has no Workshop, so install Mei from the **Catalog** tab in Anvil Manager, or from the release zip on [GitHub](https://github.com/happy-dwarf-team/mei/releases). The same mark, whichever gate you came through.

### Why "Mei"?
銘 (*mei* in Japanese) is the signature a swordsmith cuts into the tang of a blade. The same character, 铭 (*míng*) in Chinese and 銘 (*myeong*, 명) in Korean, means "inscription": one character that all three languages share, as Mei serves all three. Our team names come from the forge.

### Something's wrong. Where do I ask?
In **#help-and-bugs** on our [Discord server](https://discord.happydwarf.dev/) (pick the Mei tag), or in a [GitHub issue](https://github.com/happy-dwarf-team/mei/issues/new/choose). What we confirm on Discord goes to GitHub too.

### What is that little dwarf head on the thumbnail?
If you check the mod's GitHub page, you will notice the repository belongs to Happy Dwarf Team.

I like dwarves.
I like being happy.
I am, in fact, a happy dwarf myself.

![](https://anvil.happydwarf.dev/workshop/mei-credits.png)

**VulonLok** ([EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch), MIT) for the text, search and input logic, built unchanged from their source. Thank you!
**Anvil** for letting a dwarf into the forge.
**Source Han Sans** by Adobe (SIL Open Font Licence 1.1) for the fonts.
Mei's own code is MIT. Not affiliated with Paradox Interactive.

[![](https://anvil.happydwarf.dev/workshop/collection-en.png)](https://steamcommunity.com/sharedfiles/filedetails/?id=3833899418)
