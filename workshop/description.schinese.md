**让《欧陆风云4》显示中文、日文和韩文，不再需要旧的双字节补丁。**

Mei 让游戏能够显示、换行、搜索和输入中文、日文或韩文汉化 Mod 的文字。Mei 本身不是汉化，所以请同时订阅一个汉化 Mod。

## 你将得到
- **文字正常显示：** 菜单、提示框、事件和地图都能显示。为旧双字节补丁制作的汉化包无需修改即可使用。
- **正确换行：** 在字与字之间换行，长段落也不会再闪退。
- **自带字体：** 游戏自带的字体没有中日韩文字，凡是要用这些字体的地方，Mei 会换上用思源黑体（Source Han Sans）制作的字体，约 13,000 个字。
- **搜索和输入：** 外交国家列表和省份查找支持汉字、拼音和首字母；键盘输入或通过输入法上屏的文字可以进入文本框。

![](https://i.imgur.com/Uf8Z21n.png)

## 工作原理
游戏用一种旧的西文编码保存文字，所以没有辅助工具就无法显示中文、日文或韩文翻译。多年来，这个辅助工具是放进游戏目录的"双字节补丁"（EU4DLL）。Mei 以 [Anvil](https://anvil.happydwarf.dev/) 插件的形式完成同样的工作：

- **编码。** 游戏以 UTF-8 保存并显示文字，包括菜单、提示框、事件和地图。为旧补丁制作的汉化包（带有它的转义编码）会在加载时转换为 UTF-8，无需修改即可使用。脚本文件和存档也一样：汉化补充包放在脚本中的名称会正常显示。
- **换行。** 中文和日文没有空格，游戏原本找不到换行的位置（遇到很长的段落甚至会闪退）。Mei 按照 Unicode 的规则在字与字之间换行，逗号和句号不会出现在行首。
- **字体。** 汉化 Mod 自带的字体照常使用。游戏自带的字体没有中日韩文字，凡是游戏要用这些字体的地方，Mei 会换上用思源黑体（Source Han Sans）制作的字体（约 13,000 个字：简体和繁体中文、日文假名和汉字、韩文谚文）。
- **搜索。** 外交国家列表和省份查找支持汉字、全拼、首字母和混合输入，较长的拼音允许一个错字。
- **输入。** 键盘输入或通过输入法上屏的文字会以 UTF-8 进入文本框，退格、删除和方向键按字移动；选择文字、撤销（Ctrl+Z、Ctrl+Shift+Z）和粘贴也按字处理。

![](https://i.imgur.com/Uf8Z21n.png)

## Mei 不是汉化
你还需要一个汉化 Mod。我们用以下 Mod 测试过 Mei（每次只开 Mei 和该 Mod，使用默认设置）：
- 简体中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)
- 简体中文名称：[Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)（在上述 Mod 之上）
- 繁体中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)
- 韩文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)

创意工坊上没有找到适用于 1.37 的日文翻译；任何包含日文的 Mod，Mei 都能显示其文字。

![](https://i.imgur.com/Uf8Z21n.png)

## 安装
Mei 适用于 EU4 1.37.5（Steam，Windows）和 GOG 版 1.37.5.2。GOG 没有创意工坊，请在 Anvil Manager 的 **Catalog** 选项卡中安装 Mei，或使用 [GitHub](https://github.com/happy-dwarf-team/mei/releases) 上的发布版本压缩包。

**请先移除旧的双字节补丁**（它放进游戏目录的 `version.dll`、`d3d9.dll` 和 `plugins` 文件夹）。它和 Anvil 都使用 `version.dll`，两者只能保留一个。

1. **安装 [Anvil](https://anvil.happydwarf.dev/) 1.9 或更新版本。** 下载并运行，然后点击 **Install Anvil**。如果 Windows 提示文件有风险，请选择**更多信息**，再选择**仍要运行**。只需操作一次，创意工坊无法替你完成。
2. **订阅** Mei 和你的汉化 Mod，并在播放集中同时启用。
3. **启动游戏。** Anvil 会提示有新插件等待批准：按 **Ctrl+Shift+F12**，打开 **Mods** 并批准 Mei。
4. **重启游戏。** 无需任何设置：默认设置已经为汉化 Mod 调好。

![](https://i.imgur.com/Uf8Z21n.png)

## 尚未完成
- **输入法候选窗口：** 输入法只测试过已上屏的文字；候选窗口和正在输入的拼音尚未测试。
- Mei 自带字体中的**日文汉字**采用简体中文字形（思源黑体 SC）。
- **Anvil 自己的面板和管理器** 从 Anvil 1.9 起支持中文、日文和韩文；Anvil 1.8 只显示拉丁字母。

![](https://i.imgur.com/Uf8Z21n.png)

## 为什么叫"Mei"
銘（日文读作 *mei*）是刀匠刻在刀茎上的署名。同一个字在中文里写作"铭"（*míng*），在韩文里是 명（*myeong*），意思都是"铭文"：中日韩三种语言共用这一个字，正如 Mei 服务于这三种语言。我们团队的名字都来自铁匠铺。

![](https://i.imgur.com/Uf8Z21n.png)

## 致谢与许可
Mei 移植自 VulonLok（YozoraTempest）的 [EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch)（MIT 许可）：文字、搜索和输入的逻辑都来自原作者，从其源代码原样构建。衷心感谢！Mei 自己的代码同样采用 MIT 许可。字体：Adobe 的思源黑体（Source Han Sans），SIL 开放字体许可证 1.1。本项目与 Paradox Interactive 无关。

![](https://i.imgur.com/Uf8Z21n.png)

## Happy Dwarf Team 的其他作品
- **[Anvil](https://anvil.happydwarf.dev/)**：让 Mei 这样的插件进入 EU4 的免费加载器，附带用于批准和更新插件的 Manager。
- **[Bellows](https://steamcommunity.com/sharedfiles/filedetails/?id=3823747170)**：游戏启动时间从约 41 秒缩短到约 13 秒。
- **[Hamweard](https://steamcommunity.com/sharedfiles/filedetails/?id=3832650792)**：退出到主菜单时无需重启游戏。
- **[Tongs](https://steamcommunity.com/sharedfiles/filedetails/?id=3826695093)**：面向 Mod 测试者和工具开发者，让脚本和 AI 代理看到游戏并操作按钮。
- **[Etch](https://etch.happydwarf.dev/)**：面向界面 Mod 作者，在 VS Code 中按游戏的样子预览 `.gui` 窗口。
- 我们的全部作品：[happydwarf.dev](https://happydwarf.dev/)

![](https://i.imgur.com/Uf8Z21n.png)

### 其他下载方式
[GitHub](https://github.com/happy-dwarf-team/mei)，也适用于 GOG 版。插件页面：[anvil.happydwarf.dev/plugins/mei](https://anvil.happydwarf.dev/plugins/mei/)。
