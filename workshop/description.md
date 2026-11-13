![](https://anvil.happydwarf.dev/workshop/mei-hero.png)

Mei lets the game show, wrap, search and type the text of a Chinese, Japanese or Korean translation mod. It does not translate the game itself, so subscribe to a translation mod as well. Mei replaces the old double-byte patch (EU4DLL); the two cannot be used together.

![](https://anvil.happydwarf.dev/workshop/mei-install.png)

![](https://anvil.happydwarf.dev/workshop/mei-steps.png)

**Anvil Manager:** `AnvilManager.exe` from [GitHub](https://github.com/happy-dwarf-team/anvil/releases/latest), or the same file from our mirror, https://dl.happydwarf.dev/anvil/latest/AnvilManager.exe. If Windows says "Windows protected your PC", choose **More info**, then **Run anyway**.

**Old double-byte patch?** No need to delete anything by hand. The Manager finds EU4DLL or the original EU4 Unicode Patch in the game folder and offers **Replace EU4DLL** (or **Replace EU4 Unicode Patch**) instead of Install Anvil: click it. Their files move to `anvil-legacy-backup` in the game folder, and uninstalling Anvil can put them back.

Mei needs Anvil 1.16 or newer (take the latest; older Managers don't replace the old patch) and EU4 1.37.5 on Steam, or the GOG builds 1.37.5.2 and 1.37.5.0 (see the FAQ).

![](https://anvil.happydwarf.dev/workshop/mei-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-extra0.png)

We tested Mei with these, each with Mei alone and the default settings:
- Simplified Chinese: [Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733) (Workshop ID 2976470733)
- Simplified Chinese names: [Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990) (1999055990), on top of the mod above
- Traditional Chinese: [Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432) (2437805432)
- Korean: [Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430) (2714934430)

We found no Japanese translation for EU4 1.37 on the Workshop. Mei shows our Japanese test text, but no full translation was tried.

Mei also keeps save names in your language (`奥斯曼1444_11_11.eu4` instead of `aaa1444_11_11.eu4`), lets your Steam friends read your country's name, and writes dates and the titles of battles and sieges in the translation's word order (`1719年3月17日` instead of `17 3月 1719`, `Medine战役`), as EU4DLL did. The dates were tested with the Simplified Chinese mod on Steam; Korean dates not yet in the game.

## If something is wrong

| What you see | Why, and what to do |
| --- | --- |
| Garbled text or question marks | Mei is not running. Check that you approved it (step 3) and that the translation mod is in the playset. |
| The game closes to the desktop at start | Usually a translation mod's package put the old patch back. Open Anvil Manager and click **Replace** again: your plugins and settings are still there. |
| The Manager says `version.dll` is not Anvil's | Another mod or tool put its own `version.dll` there. Only one can be there: tick **Replace the other version.dll** and install Anvil. |
| Mei says "waiting for approval" | Approve it as in step 3 and restart the game. |

Still stuck? Leave a comment below (see "Something's wrong. Where do I ask?" in the FAQ).

![](https://anvil.happydwarf.dev/workshop/mei-extra1.png)

- **Switching is one-way:** saves made with Mei show garbled names in a game with EU4DLL, and a multiplayer group switches to Mei together.
- **IME:** no real IME was used. The tests sent finished text to the window, so the candidate window and the text you are still composing were not checked.
- **Japanese kanji** in Mei's own fonts take their Simplified Chinese shapes (Source Han Sans SC).

[![](https://anvil.happydwarf.dev/workshop/mei-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-github.png)](https://github.com/happy-dwarf-team/mei)

[![](https://anvil.happydwarf.dev/workshop/discord-en.png)](https://discord.happydwarf.dev/)

![](https://anvil.happydwarf.dev/workshop/mei-faq.png)

### Why do I have to approve it?
Mei is a real program running inside your game, not just a pile of text files. Anvil makes sure nothing like that runs without your nod. Trust, but verify. Dwarf law.

### I play on GOG. Can I use it?
Yes, on the GOG builds 1.37.5.2 and 1.37.5.0, with the latest Anvil. GOG has no Workshop, so install Mei from the **Catalog** tab in Anvil Manager, or from the release zip on [GitHub](https://github.com/happy-dwarf-team/mei/releases). The same mark, whichever gate you came through.

### Why "Mei"?
銘 (*mei* in Japanese) is the signature a swordsmith cuts into the tang of a blade. The same character, 铭 (*míng*) in Chinese and 銘 (*myeong*, 명) in Korean, means "inscription": one character that all three languages share, as Mei serves all three. Our team names come from the forge.

### Something's wrong. Where do I ask?
**Leave a comment below** first, or ask in the help thread under this item's Discussions; Chinese is welcome. Or open a [GitHub issue](https://github.com/happy-dwarf-team/mei/issues/new/choose), in Chinese too, with `anvil/anvil.log` from the game folder and the translation mods you use. Our [Discord server](https://discord.happydwarf.dev/) is in English (**#help-and-bugs**, pick the Mei tag).

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
