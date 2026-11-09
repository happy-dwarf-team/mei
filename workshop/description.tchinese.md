![](https://anvil.happydwarf.dev/workshop/mei-tchinese-hero.png)

Mei 讓遊戲能夠顯示、換行、搜尋和輸入中文、日文或韓文中文化 Mod 的文字。Mei 本身不是中文化，所以請同時訂閱一個中文化 Mod。Mei 取代舊的雙字節補丁（EU4DLL），兩者不能同時使用。

本說明的每一步都寫成文字，圖片只作補充，圖片打不開也能照著做。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-install.png)

1. **刪除舊的雙字節補丁。** 在 Steam 收藏庫中以右鍵點擊 Europa Universalis IV，選擇「管理」→「瀏覽本機檔案」，刪除遊戲資料夾中的 `version.dll`、`d3d9.dll` 和 `plugins` 資料夾（EU4DLL 或原版 EU4 Unicode Patch 放進去的檔案）。舊補丁和 Anvil 都使用 `version.dll`，兩者只能保留一個。
2. **下載 Anvil Manager。** 從 GitHub 下載 `AnvilManager.exe`：https://github.com/happy-dwarf-team/anvil/releases/latest 。如果 GitHub 打不開，請稍後重試或使用加速器。
3. **執行 Manager。** 如果 Windows 顯示「Windows 已保護您的電腦」，請點擊 **其他資訊**，再點擊 **仍要執行**。
4. **安裝 Anvil。** Manager 會找到遊戲資料夾。在 **Anvil** 分頁中點擊 **Install Anvil**（安裝 Anvil）。如果它提示 `version.dll` 不屬於 Anvil，表示舊補丁的檔案還在：讓 Manager 取代它，或回到第 1 步。需要 Anvil 1.9 或更新版本。
5. **訂閱 Mei 和一個中文化 Mod**（見下文），並在啟動器的遊玩組合（Playset）中同時啟用它們。
6. **核准 Mei。** 啟動遊戲，按 `Ctrl+Shift+F12` 開啟 Anvil 面板，在 **Mods**（模組）分頁中核准 Mei，然後重新啟動遊戲。也可以在 Manager 的 **Mods** 分頁中核准。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-steps.png)

支援 EU4 1.37.5（Steam），也支援 GOG 版 1.37.5.2 和 1.37.5.0（見常見問題）。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-extra0.png)

我們用以下 Mod 測試過 Mei（每次只開 Mei 和該 Mod，使用預設設定）：
- 簡體中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)（工作坊編號 2976470733）
- 簡體中文名稱：[Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)（1999055990，在上述 Mod 之上）
- 繁體中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)（2437805432）
- 韓文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)（2714934430）

工作坊上沒有找到適用於 1.37 的日文翻譯。Mei 能顯示我們的日文測試文字，但尚未用完整的翻譯測試。

## 發生問題時

| 現象 | 原因和解決方法 |
| --- | --- |
| 文字顯示為亂碼或問號 | Mei 沒有執行。請確認已經核准 Mei（第 6 步）、中文化 Mod 已在遊玩組合中啟用，並且舊補丁已經刪除。 |
| 遊戲啟動後跳回桌面 | 通常是舊補丁的殘留，或中文化 Mod 的安裝包又放回了 `version.dll`。請再次刪除第 1 步中的檔案，然後在 Manager 中重新安裝 Anvil。 |
| Manager 提示 `version.dll` 不屬於 Anvil | 這是舊補丁的 `version.dll`。讓 Manager 取代它，或先手動刪除。 |
| Mei 顯示「等待核准」（waiting） | Mei 還沒有被核准。請按第 6 步核准，然後重新啟動遊戲。 |

仍未解決？請在本頁面下方留言（見常見問題「遇到問題，去哪裡提問？」）。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-extra1.png)

- **改用 Mei 是單向的：** 用 Mei 儲存的存檔，在裝有 EU4DLL 的遊戲中名稱會顯示為亂碼。連線遊戲時，所有玩家需要一起改用 Mei。
- **輸入法：** 沒有用真實的輸入法測試。測試時直接向視窗傳送已完成的文字，所以候選視窗和正在組字的內容尚未測試。
- Mei 自帶字型中的**日文漢字**採用簡體中文字形（思源黑體 SC）。
- **Anvil 自己的面板和管理器** 從 Anvil 1.9 起支援中文、日文和韓文；Anvil 1.8 只顯示拉丁字母。

[![](https://anvil.happydwarf.dev/workshop/mei-tchinese-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-tchinese-github.png)](https://github.com/happy-dwarf-team/mei)

[![](https://anvil.happydwarf.dev/workshop/discord-tchinese.png)](https://discord.happydwarf.dev/)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-faq.png)

### 為什麼需要核准？
Mei 是在遊戲內執行的真正程式，不只是一堆文字檔。Anvil 確保這樣的程式不會未經你點頭就執行。信任，但要查證。這是矮人的規矩。

### 我玩的是 GOG 版，可以用嗎？
可以，支援 GOG 版 1.37.5.2 和 1.37.5.0（1.37.5.0 需要 Anvil 1.15 或更新版本）。GOG 沒有工作坊，請在 Anvil Manager 的 **Catalog**（目錄）分頁安裝 Mei，或使用 GitHub 上的發行版本壓縮檔：https://github.com/happy-dwarf-team/mei/releases

### 為什麼叫「Mei」？
銘（日文讀作 *mei*）是刀匠刻在刀莖上的署名。同一個字在中文裡讀作 *míng*，在韓文裡是 명（*myeong*），意思都是「銘文」：中日韓三種語言共用這一個字，正如 Mei 服務於這三種語言。我們團隊的名字都來自鐵匠鋪。

### 遇到問題，去哪裡提問？
請先**在本頁面下方留言**，或在本頁面「討論」中的求助串裡提問，可以直接用中文。也可以在 [GitHub](https://github.com/happy-dwarf-team/mei/issues/new/choose) 上用中文回報問題（請附上遊戲資料夾中的 `anvil/anvil.log`，並寫明你使用的中文化 Mod）。我們的 [Discord 伺服器](https://discord.happydwarf.dev/)使用英語（**#help-and-bugs** 論壇，Mei 標籤）。

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
