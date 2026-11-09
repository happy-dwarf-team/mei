![](https://anvil.happydwarf.dev/workshop/mei-schinese-hero.png)

Mei 让游戏能够显示、换行、搜索和输入中文、日文或韩文汉化 Mod 的文字。Mei 本身不是汉化，所以请同时订阅一个汉化 Mod。Mei 取代旧的双字节补丁（EU4DLL），两者不能同时使用。

本说明的每一步都写成文字，图片只作补充，图片打不开也能照着做。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-install.png)

1. **删除旧的双字节补丁。** 在 Steam 库中右键点击 Europa Universalis IV，选择“管理” → “浏览本地文件”，删除游戏目录中的 `version.dll`、`d3d9.dll` 和 `plugins` 文件夹（EU4DLL 或原版 EU4 Unicode Patch 放进去的文件）。旧补丁和 Anvil 都使用 `version.dll`，两者只能保留一个。
2. **下载 Anvil Manager。** 从 GitHub 下载 `AnvilManager.exe`：https://github.com/happy-dwarf-team/anvil/releases/latest 。GitHub 在国内有时打不开，请稍后重试或使用加速器。
3. **运行 Manager。** 如果 Windows 显示“Windows 已保护你的电脑”，请点击 **更多信息**，再点击 **仍要运行**。
4. **安装 Anvil。** Manager 会找到游戏目录。在 **Anvil** 选项卡中点击 **Install Anvil**（安装 Anvil）。如果它提示 `version.dll` 不属于 Anvil，说明旧补丁的文件还在：让 Manager 替换它，或者回到第 1 步。需要 Anvil 1.9 或更新版本。
5. **订阅 Mei 和一个汉化 Mod**（见下文），并在启动器的播放集（Playset）中同时启用它们。
6. **批准 Mei。** 启动游戏，按 `Ctrl+Shift+F12` 打开 Anvil 面板，在 **Mods**（模组）选项卡中批准 Mei，然后重启游戏。也可以在 Manager 的 **Mods** 选项卡中批准。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-steps.png)

支持 EU4 1.37.5（Steam），也支持 GOG 版 1.37.5.2（见常见问题）。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-extra0.png)

我们用以下 Mod 测试过 Mei（每次只开 Mei 和该 Mod，使用默认设置）：
- 简体中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)（创意工坊编号 2976470733）
- 简体中文名称：[Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)（1999055990，在上述 Mod 之上）
- 繁体中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)（2437805432）
- 韩文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)（2714934430）

创意工坊上没有找到适用于 1.37 的日文翻译。Mei 能显示我们的日文测试文字，但尚未用完整的翻译测试。

## 出现问题时

| 现象 | 原因和解决办法 |
| --- | --- |
| 文字显示为乱码或问号 | Mei 没有运行。请确认已经批准 Mei（第 6 步）、汉化 Mod 已在播放集中启用，并且旧补丁已经删除。 |
| 游戏启动后退回桌面 | 通常是旧补丁的残留，或者汉化 Mod 的安装包又放回了 `version.dll`。请再次删除第 1 步中的文件，然后在 Manager 中重新安装 Anvil。 |
| Manager 提示 `version.dll` 不属于 Anvil | 这是旧补丁的 `version.dll`。让 Manager 替换它，或者先手动删除。 |
| Mei 显示“等待批准”（waiting） | Mei 还没有被批准。请按第 6 步批准，然后重启游戏。 |

仍未解决？请在本页面下方留言（见常见问题“遇到问题，去哪里提问？”）。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-extra1.png)

- **改用 Mei 是单向的：** 用 Mei 保存的存档，在装有 EU4DLL 的游戏中名称会显示为乱码。联机时，所有玩家需要一起改用 Mei。
- **输入法：** 没有用真实的输入法测试。测试时直接向窗口发送已完成的文字，所以候选窗口和正在输入的拼音尚未测试。
- Mei 自带字体中的**日文汉字**采用简体中文字形（思源黑体 SC）。
- **Anvil 自己的面板和管理器** 从 Anvil 1.9 起支持中文、日文和韩文；Anvil 1.8 只显示拉丁字母。

[![](https://anvil.happydwarf.dev/workshop/mei-schinese-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-schinese-github.png)](https://github.com/happy-dwarf-team/mei)

[![](https://anvil.happydwarf.dev/workshop/discord-schinese.png)](https://discord.happydwarf.dev/)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-faq.png)

### 为什么需要批准？
Mei 是在游戏内运行的真正程序，不只是一堆文本文件。Anvil 确保这样的程序不会未经你点头就运行。信任，但要核实。这是矮人的规矩。

### 我玩的是 GOG 版，可以用吗？
可以，支持 GOG 版 1.37.5.2。GOG 没有创意工坊，请在 Anvil Manager 的 **Catalog**（目录）选项卡中安装 Mei，或使用 GitHub 上的发布版本压缩包：https://github.com/happy-dwarf-team/mei/releases

### 为什么叫"Mei"？
銘（日文读作 *mei*）是刀匠刻在刀茎上的署名。同一个字在中文里写作"铭"（*míng*），在韩文里是 명（*myeong*），意思都是"铭文"：中日韩三种语言共用这一个字，正如 Mei 服务于这三种语言。我们团队的名字都来自铁匠铺。

### 遇到问题，去哪里提问？
请先**在本页面下方留言**，或在本页面“讨论”中的求助帖里提问，可以直接用中文。也可以在 [GitHub](https://github.com/happy-dwarf-team/mei/issues/new/choose) 上用中文提交问题（请附上游戏目录中的 `anvil/anvil.log`，并写明你用的汉化 Mod）。我们的 [Discord 服务器](https://discord.happydwarf.dev/)使用英语（**#help-and-bugs** 论坛，Mei 标签）。

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
