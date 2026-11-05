![](https://anvil.happydwarf.dev/workshop/mei-tchinese-hero.png)

Mei 讓遊戲能夠顯示、換行、搜尋和輸入中文、日文或韓文中文化 Mod 的文字。Mei 本身不是中文化，所以請同時訂閱一個中文化 Mod。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-install.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-steps.png)

**請先移除舊的雙字節補丁**（它放進遊戲資料夾的 `version.dll`、`d3d9.dll` 和 `plugins` 資料夾）。它和 Anvil 都使用 `version.dll`，兩者只能保留一個。

需要 [Anvil](https://anvil.happydwarf.dev/) 1.9 或更新版本和 EU4 1.37.5（Steam；也支援 GOG 版 1.37.5.2，見常見問題）。如果 Windows 提示檔案有風險，請選擇**其他資訊**，再選擇**仍要執行**。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-extra0.png)

你還需要一個中文化 Mod。我們用以下 Mod 測試過 Mei（每次只開 Mei 和該 Mod，使用預設設定）：
- 簡體中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)
- 簡體中文名稱：[Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)（在上述 Mod 之上）
- 繁體中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)
- 韓文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)

工作坊上沒有找到適用於 1.37 的日文翻譯；任何包含日文的 Mod，Mei 都能顯示其文字。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-extra1.png)

- **輸入法候選視窗：** 輸入法只測試過已送出的文字；候選視窗和正在組字的內容尚未測試。
- Mei 自帶字型中的**日文漢字**採用簡體中文字形（思源黑體 SC）。
- **Anvil 自己的面板和管理器** 從 Anvil 1.9 起支援中文、日文和韓文；Anvil 1.8 只顯示拉丁字母。

[![](https://anvil.happydwarf.dev/workshop/mei-tchinese-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-tchinese-github.png)](https://github.com/happy-dwarf-team/mei)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-faq.png)

### 為什麼需要核准？
Mei 是在遊戲內執行的真正程式，不只是一堆文字檔。Anvil 確保這樣的程式不會未經你點頭就執行。信任，但要查證。這是矮人的規矩。

### 我玩的是 GOG 版，可以用嗎？
可以，支援 GOG 版 1.37.5.2。GOG 沒有工作坊，請在 Anvil Manager 的 **Catalog** 分頁安裝 Mei，或使用 [GitHub](https://github.com/happy-dwarf-team/mei/releases) 上的發行版本壓縮檔。

### 為什麼叫「Mei」？
銘（日文讀作 *mei*）是刀匠刻在刀莖上的署名。同一個字在中文裡讀作 *míng*，在韓文裡是 명（*myeong*），意思都是「銘文」：中日韓三種語言共用這一個字，正如 Mei 服務於這三種語言。我們團隊的名字都來自鐵匠鋪。

### 縮圖上那個小矮人頭像是什麼？
看看這個 Mod 的 GitHub 頁面，你會發現儲存庫屬於 Happy Dwarf Team。

我喜歡矮人。
我喜歡開心。
其實我本人就是一個開心的矮人。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-credits.png)

**VulonLok**（[EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch)，MIT 授權）：文字、搜尋和輸入的邏輯都來自原作者，從其原始碼原樣建置。衷心感謝！
**Anvil** 讓矮人走進了鐵匠鋪。
**思源黑體**（Source Han Sans，Adobe，SIL 開放字型授權 1.1）提供字型。
Mei 自己的程式碼採用 MIT 授權。本專案與 Paradox Interactive 無關。

[![](https://anvil.happydwarf.dev/workshop/collection-tchinese.png)](https://steamcommunity.com/sharedfiles/filedetails/?id=3833899418)
