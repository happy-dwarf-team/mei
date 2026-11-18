![](https://anvil.happydwarf.dev/workshop/mei-schinese-hero.png)

Mei 让游戏能够显示、换行、搜索和输入中文、日文或韩文汉化 Mod 的文字。Mei 本身不是汉化，所以请同时订阅一个汉化 Mod。Mei 取代旧的双字节补丁（EU4DLL），两者不能同时使用。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-install.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-steps.png)

**Anvil Manager：** 镜像（国内推荐）：https://dl.happydwarf.dev/anvil/latest/AnvilManager.exe 。这是 GitHub 上同一文件的镜像，每次发布后自动复制，并核对 SHA-256 一致。原始地址（GitHub）：https://github.com/happy-dwarf-team/anvil/releases/latest 。如果一个链接打不开，请试另一个，或稍后重试。如果 Windows 显示“Windows 已保护你的电脑”，请点击 **更多信息**，再点击 **仍要运行**。

需要 Anvil 1.16 或更新版本（建议用最新版）。支持 EU4 1.37.5（Steam），也支持 GOG 版 1.37.5.2 和 1.37.5.0（见常见问题）。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-extra0.png)

我们用以下 Mod 测试过 Mei（每次只开 Mei 和该 Mod，使用默认设置）：
- 简体中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)（创意工坊编号 2976470733）
- 简体中文名称：[Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)（1999055990，在上述 Mod 之上）
- 繁体中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)（2437805432）
- 韩文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)（2714934430）

创意工坊上没有找到适用于 1.37 的日文翻译。Mei 能显示我们的日文测试文字，但尚未用完整的翻译测试。

Mei 还让存档名保留原文（如 `奥斯曼1444_11_11.eu4`），Steam 好友能看懂你的国家名，日期和战役标题也按汉化的语序显示（`1719年3月17日`、`Medine战役`）。日期已在游戏中用 Steam 上的简体中文 Mod 和 Korean Language Mod 测试。Korean Language Mod 的日期从 0.8.0 之后的 Mei 版本起按它的语序显示，它的战役标题保持游戏原来的顺序。

## 出现问题时

| 现象 | 原因和解决办法 |
| --- | --- |
| 文字显示为乱码或问号 | Mei 没有运行。请确认已经批准 Mei（第 3 步），并且汉化 Mod 已在播放集中启用。 |
| 游戏启动后退回桌面 | 通常是汉化 Mod 的安装包自带了双字节补丁。请打开 Anvil Manager，点击 **替换**：你的插件和设置都会保留。 |
| Manager 提示 `version.dll` 不属于 Anvil | 其他 Mod 或工具放了自己的 `version.dll`。两者只能保留一个：勾选 **替换另一个 version.dll**，然后安装 Anvil。 |
| Mei 显示“等待批准”（waiting） | Mei 还没有被批准。请按第 3 步批准，然后重启游戏。 |

仍未解决？请在本页面下方留言（见常见问题“遇到问题，去哪里提问？”）。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-extra1.png)

- **输入法：** 没有用真实的输入法测试。测试时直接向窗口发送已完成的文字，所以候选窗口和正在输入的拼音尚未测试。
- Mei 自带字体中的**日文汉字**采用简体中文字形（思源黑体 SC）。

[![](https://anvil.happydwarf.dev/workshop/mei-schinese-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-schinese-github.png)](https://github.com/happy-dwarf-team/mei)

[![](https://anvil.happydwarf.dev/workshop/discord-schinese.png)](https://discord.happydwarf.dev/)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-faq.png)

### 为什么需要批准？
Mei 是在游戏内运行的真正程序，不只是一堆文本文件。Anvil 确保这样的程序不会未经你点头就运行。信任，但要核实。这是矮人的规矩。

### 以前用过 EU4DLL 或 EU4 Unicode Patch，怎么办？
在 Anvil Manager 中点击 **替换 EU4DLL**（或 **替换 EU4 Unicode Patch**）：旧文件会移到 `anvil-legacy-backup`，卸载 Anvil 时可以放回原处。用 Mei 保存的存档在 EU4DLL 中名称会显示为乱码，所以联机时所有玩家要一起改用 Mei。

### 我玩的是 GOG 版，可以用吗？
可以，支持 1.37.5.2 和 1.37.5.0：GOG 没有创意工坊，请在 Anvil Manager 的 **Catalog**（目录）页面中安装。无论从哪道门进来，都是同一个铭。

### 我在 Linux 上玩，可以用吗？
大概可以，需通过 Proton：Anvil 已在桌面版 Linux 上运行，但 Mei 及其中文文本尚未在 Proton 下测试，Steam Deck 也未测试。请为 EU4 强制使用 Proton，用压缩包安装 Anvil，并添加启动选项 `WINEDLLOVERRIDES="version=n,b" %command%`（[步骤](https://anvil.happydwarf.dev/docs/install/#on-linux-with-proton)）。在 Proton 下批准创意工坊插件尚未尝试；如不成功，请手动安装插件的压缩包。

### 为什么叫"Mei"？
銘（日文读作 *mei*）是日本刀匠刻在刀上的署名，在中文（铭，*míng*）和韩文（명，*myeong*）里意思是"铭文"：Mei 服务的三种语言共用这一个字。

### 遇到问题，去哪里提问？
请**在本页面下方留言**，或在 [GitHub](https://github.com/happy-dwarf-team/mei/issues/new/choose) 上提交问题并附上游戏目录中的 `anvil/anvil.log`，两处都可以用中文。我们的 [Discord 服务器](https://discord.happydwarf.dev/)使用英语（**#help-and-bugs**，Mei 标签）。

### 缩略图上那个小矮人头像是什么？
看看这个 Mod 的 GitHub 页面，你会发现仓库属于 Happy Dwarf Team。

我喜欢矮人。
我喜欢开心。
其实我本人就是一个开心的矮人。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-credits.png)

**VulonLok**（[EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch)，MIT 许可）：文字、搜索和输入的逻辑都来自原作者，从其源代码原样构建。衷心感谢！
**Anvil** 让矮人走进了铁匠铺。
**思源黑体**（Source Han Sans，Adobe，SIL 开放字体许可证 1.1）提供字体。
Mei 自己的代码采用 MIT 许可。本项目与 Paradox Interactive 无关。

[![](https://anvil.happydwarf.dev/workshop/collection-schinese.png)](https://steamcommunity.com/sharedfiles/filedetails/?id=3833899418)
