![](https://anvil.happydwarf.dev/workshop/mei-schinese-hero.png)

Mei 让游戏能够显示、换行、搜索和输入中文、日文或韩文汉化 Mod 的文字。Mei 本身不是汉化，所以请同时订阅一个汉化 Mod。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-install.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-steps.png)

**请先移除旧的双字节补丁**（它放进游戏目录的 `version.dll`、`d3d9.dll` 和 `plugins` 文件夹）。它和 Anvil 都使用 `version.dll`，两者只能保留一个。

需要 [Anvil](https://anvil.happydwarf.dev/) 1.9 或更新版本和 EU4 1.37.5（Steam；也支持 GOG 版 1.37.5.2，见常见问题）。如果 Windows 提示文件有风险，请选择**更多信息**，再选择**仍要运行**。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-extra0.png)

你还需要一个汉化 Mod。我们用以下 Mod 测试过 Mei（每次只开 Mei 和该 Mod，使用默认设置）：
- 简体中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)
- 简体中文名称：[Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)（在上述 Mod 之上）
- 繁体中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)
- 韩文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)

创意工坊上没有找到适用于 1.37 的日文翻译；任何包含日文的 Mod，Mei 都能显示其文字。

![](https://anvil.happydwarf.dev/workshop/mei-schinese-extra1.png)

- **输入法候选窗口：** 输入法只测试过已上屏的文字；候选窗口和正在输入的拼音尚未测试。
- Mei 自带字体中的**日文汉字**采用简体中文字形（思源黑体 SC）。
- **Anvil 自己的面板和管理器** 从 Anvil 1.9 起支持中文、日文和韩文；Anvil 1.8 只显示拉丁字母。

[![](https://anvil.happydwarf.dev/workshop/mei-schinese-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-schinese-github.png)](https://github.com/happy-dwarf-team/mei)

![](https://anvil.happydwarf.dev/workshop/mei-schinese-faq.png)

### 为什么需要批准？
Mei 是在游戏内运行的真正程序，不只是一堆文本文件。Anvil 确保这样的程序不会未经你点头就运行。信任，但要核实。这是矮人的规矩。

### 我玩的是 GOG 版，可以用吗？
可以，支持 GOG 版 1.37.5.2。GOG 没有创意工坊，请在 Anvil Manager 的 **Catalog** 选项卡中安装 Mei，或使用 [GitHub](https://github.com/happy-dwarf-team/mei/releases) 上的发布版本压缩包。

### 为什么叫"Mei"？
銘（日文读作 *mei*）是刀匠刻在刀茎上的署名。同一个字在中文里写作"铭"（*míng*），在韩文里是 명（*myeong*），意思都是"铭文"：中日韩三种语言共用这一个字，正如 Mei 服务于这三种语言。我们团队的名字都来自铁匠铺。

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
