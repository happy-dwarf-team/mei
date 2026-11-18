![](https://anvil.happydwarf.dev/workshop/mei-tchinese-hero.png)

Mei 讓遊戲能夠顯示、換行、搜尋和輸入中文、日文或韓文中文化 Mod 的文字。Mei 本身不是中文化，所以請同時訂閱一個中文化 Mod。Mei 取代舊的雙字節補丁（EU4DLL），兩者不能同時使用。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-install.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-steps.png)

**Anvil Manager：** 鏡像：https://dl.happydwarf.dev/anvil/latest/AnvilManager.exe 。這是 GitHub 上同一檔案的鏡像，每次發行後自動複製，並核對 SHA-256 一致。原始位址（GitHub）：https://github.com/happy-dwarf-team/anvil/releases/latest 。如果一個連結打不開，請試另一個，或稍後重試。如果 Windows 顯示「Windows 已保護您的電腦」，請點擊 **其他資訊**，再點擊 **仍要執行**。

需要 Anvil 1.16 或更新版本（建議用最新版）。支援 EU4 1.37.5（Steam），也支援 GOG 版 1.37.5.2 和 1.37.5.0（見常見問題）。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-tiles-title.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-tiles.png)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-extra0.png)

我們用以下 Mod 測試過 Mei（每次只開 Mei 和該 Mod，使用預設設定）：
- 簡體中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)（工作坊編號 2976470733）
- 簡體中文名稱：[Chinese Language Supplementary Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=1999055990)（1999055990，在上述 Mod 之上）
- 繁體中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)（2437805432）
- 韓文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)（2714934430）

工作坊上沒有找到適用於 1.37 的日文翻譯。Mei 能顯示我們的日文測試文字，但尚未用完整的翻譯測試。

Mei 還讓存檔名保留原文（如 `奧斯曼1444_11_11.eu4`），Steam 好友能看懂你的國家名，日期和戰役標題也按譯文的語序顯示（`1719年3月17日`、`Medine戰役`）。日期已在遊戲中用 Steam 上的簡體中文 Mod 和 Korean Language Mod 測試。Korean Language Mod 的日期從 0.8.0 之後的 Mei 版本起按它的語序顯示，它的戰役標題保持遊戲原來的順序。

## 發生問題時

| 現象 | 原因和解決方法 |
| --- | --- |
| 文字顯示為亂碼或問號 | Mei 沒有執行。請確認已經核准 Mei（第 3 步），並且中文化 Mod 已在遊玩組合中啟用。 |
| 遊戲啟動後跳回桌面 | 通常是中文化 Mod 的安裝包自帶了雙字節補丁。請開啟 Anvil Manager，點擊 **取代**：你的外掛和設定都會保留。 |
| Manager 提示 `version.dll` 不屬於 Anvil | 其他 Mod 或工具放了自己的 `version.dll`。兩者只能保留一個：勾選 **取代另一個 version.dll**，然後安裝 Anvil。 |
| Mei 顯示「等待核准」（waiting） | Mei 還沒有被核准。請按第 3 步核准，然後重新啟動遊戲。 |

仍未解決？請在本頁面下方留言（見常見問題「遇到問題，去哪裡提問？」）。

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-extra1.png)

- **輸入法：** 沒有用真實的輸入法測試。測試時直接向視窗傳送已完成的文字，所以候選視窗和正在組字的內容尚未測試。
- Mei 自帶字型中的**日文漢字**採用簡體中文字形（思源黑體 SC）。

[![](https://anvil.happydwarf.dev/workshop/mei-tchinese-docs.png)](https://anvil.happydwarf.dev/plugins/mei/) [![](https://anvil.happydwarf.dev/workshop/mei-tchinese-github.png)](https://github.com/happy-dwarf-team/mei)

[![](https://anvil.happydwarf.dev/workshop/discord-tchinese.png)](https://discord.happydwarf.dev/)

![](https://anvil.happydwarf.dev/workshop/mei-tchinese-faq.png)

### 為什麼需要核准？
Mei 是在遊戲內執行的真正程式，不只是一堆文字檔。Anvil 確保這樣的程式不會未經你點頭就執行。信任，但要查證。這是矮人的規矩。

### 以前用過 EU4DLL 或 EU4 Unicode Patch，怎麼辦？
在 Anvil Manager 中點擊 **取代 EU4DLL**（或 **取代 EU4 Unicode Patch**）：舊檔案會移到 `anvil-legacy-backup`，解除安裝 Anvil 時可以放回原處。用 Mei 儲存的存檔在 EU4DLL 中名稱會顯示為亂碼，所以連線遊戲時所有玩家要一起改用 Mei。

### 我玩的是 GOG 版，可以用嗎？
可以，支援 1.37.5.2 和 1.37.5.0：GOG 沒有工作坊，請在 Anvil Manager 的 **Catalog**（目錄）頁面安裝。無論從哪道門進來，都是同一個銘。

### 我在 Linux 上玩，可以用嗎？
大概可以，需透過 Proton：Anvil 已在桌面版 Linux 上執行，但 Mei 及其中文文字尚未在 Proton 下測試，Steam Deck 也未測試。請為 EU4 強制使用 Proton，用壓縮檔安裝 Anvil，並加入啟動選項 `WINEDLLOVERRIDES="version=n,b" %command%`（[步驟](https://anvil.happydwarf.dev/docs/install/#on-linux-with-proton)）。在 Proton 下核准工作坊外掛尚未嘗試；如不成功，請手動安裝外掛的壓縮檔。

### 為什麼叫「Mei」？
銘（日文讀作 *mei*）是日本刀匠刻在刀上的署名，在中文（*míng*）和韓文（명，*myeong*）裡意思是「銘文」：Mei 服務的三種語言共用這一個字。

### 遇到問題，去哪裡提問？
請**在本頁面下方留言**，或在 [GitHub](https://github.com/happy-dwarf-team/mei/issues/new/choose) 上回報問題並附上遊戲資料夾中的 `anvil/anvil.log`，兩處都可以用中文。我們的 [Discord 伺服器](https://discord.happydwarf.dev/)使用英語（**#help-and-bugs**，Mei 標籤）。

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
