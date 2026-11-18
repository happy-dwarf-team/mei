![](https://anvil.happydwarf.dev/workshop/mei-hero.png)

Mei lets the game show, wrap, search and type the text of a Chinese, Japanese or Korean translation mod. It does not translate the game itself, so subscribe to a translation mod as well. Mei replaces the old double-byte patch (EU4DLL); the two cannot be used together.

![](https://anvil.happydwarf.dev/workshop/mei-install.png)

![](https://anvil.happydwarf.dev/workshop/mei-steps.png)

**Anvil Manager:** `AnvilManager.exe` from [GitHub](https://github.com/happy-dwarf-team/anvil/releases/latest), or the same file from our mirror, https://dl.happydwarf.dev/anvil/latest/AnvilManager.exe. If Windows says "Windows protected your PC", choose **More info**, then **Run anyway**.

Mei needs Anvil 1.16 or newer (take the latest) and EU4 1.37.5 on Steam, or the GOG builds 1.37.5.2 and 1.37.5.0 (see the FAQ).

![](https://anvil.happydwarf.dev/workshop/mei-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-extra0.png)

We tested Mei with these, each with Mei alone and the default settings:
- Simplified Chinese: [Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733) (Workshop ID 2976470733)
- Simplified Chinese names: [Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990) (1999055990), on top of the mod above
- Traditional Chinese: [Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432) (2437805432)
- Korean: [Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430) (2714934430)

We found no Japanese translation for EU4 1.37 on the Workshop. Mei shows our Japanese test text, but no full translation was tried.

Mei also keeps save names in your language (`奥斯曼1444_11_11.eu4` instead of `aaa1444_11_11.eu4`), lets your Steam friends read your country's name, and writes dates and the titles of battles and sieges in the translation's word order (`1719年3月17日` instead of `17 3月 1719`, `Medine战役`). The dates were tested in the game with the Simplified Chinese mod and the Korean Language Mod on Steam. The Korean Language Mod's dates follow its word order in Mei versions after 0.8.0, and its battle titles keep the game's order.

## If something is wrong

| What you see | Why, and what to do |
| --- | --- |
| Garbled text or question marks | Mei is not running. Check that you approved it (step 3) and that the translation mod is in the playset. |
| The game closes to the desktop at start | Usually a translation mod's package brought its own double-byte patch. Open Anvil Manager and click **Replace**: your plugins and settings stay. |
| The Manager says `version.dll` is not Anvil's | Another mod or tool put its own `version.dll` there. Only one can be there: tick **Replace the other version.dll** and install Anvil. |
| Mei says "waiting for approval" | Approve it as in step 3 and restart the game. |

Still stuck? Leave a comment below (see "Something's wrong. Where do I ask?" in the FAQ).

![](https://anvil.happydwarf.dev/workshop/mei-extra1.png)

- **IME:** no real IME was used. The tests sent finished text to the window, so the candidate window and the text you are still composing were not checked.
- **Japanese kanji** in Mei's own fonts take their Simplified Chinese shapes (Source Han Sans SC).

[![](https://anvil.happydwarf.dev/workshop/mei-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-github.png)](https://github.com/happy-dwarf-team/mei)

[![](https://anvil.happydwarf.dev/workshop/discord-en.png)](https://discord.happydwarf.dev/)

![](https://anvil.happydwarf.dev/workshop/mei-faq.png)

### Why do I have to approve it?
Mei is a real program running inside your game, not just a pile of text files. Anvil makes sure nothing like that runs without your nod. Trust, but verify. Dwarf law.

### I used EU4DLL or the EU4 Unicode Patch. What now?
Click **Replace EU4DLL** (or **Replace EU4 Unicode Patch**) in Anvil Manager: the old files move to `anvil-legacy-backup`, and uninstalling Anvil can put them back. Saves made with Mei show garbled names with EU4DLL, so a multiplayer group switches together.

### I play on GOG. Can I use it?
Yes, 1.37.5.2 and 1.37.5.0: GOG has no Workshop, so install it from the **Catalog** page of Anvil Manager. The same mark, whichever gate you came through.

### I play on Linux. Can I use it?
Probably, through Proton: Anvil runs there on desktop Linux, but nobody has tested Mei and its Chinese text under Proton yet, nor the Steam Deck. Force Proton for EU4, install Anvil from its zip and add the launch option `WINEDLLOVERRIDES="version=n,b" %command%` ([the steps](https://anvil.happydwarf.dev/docs/install/#on-linux-with-proton)). Approving a Workshop plugin there has not been tried yet; if it fails, install the plugin's zip by hand and its fonts as a small local mod, as on GOG ([Mei's page](https://anvil.happydwarf.dev/plugins/mei/)).

### Why "Mei"?
銘 (*mei*) is the signature a Japanese swordsmith cuts into a blade, and in Chinese (铭, *míng*) and Korean (명, *myeong*) it means "inscription": one character for the three languages Mei serves.

### Something's wrong. Where do I ask?
**Leave a comment below**, or open a [GitHub issue](https://github.com/happy-dwarf-team/mei/issues/new/choose) with `anvil/anvil.log` from the game folder; Chinese is welcome in both. Our [Discord server](https://discord.happydwarf.dev/) is in English (**#help-and-bugs**, pick the Mei tag).

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
