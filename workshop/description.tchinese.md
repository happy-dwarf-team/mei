**讓《歐陸風雲4》顯示中文、日文和韓文，不再需要舊的雙字節補丁。Mei 本身不是中文化：它讓遊戲能夠顯示、搜尋和輸入中文化 Mod 的文字。**

![](https://i.imgur.com/Uf8Z21n.png)

## 簡介
遊戲以一種舊的西文編碼儲存文字，因此沒有輔助工具就無法顯示中文、日文或韓文翻譯。多年來，這個輔助工具是放進遊戲資料夾的「雙字節補丁」（EU4DLL）。Mei 以 [Anvil](https://anvil.happydwarf.dev/) 外掛的形式完成同樣的工作：

- **編碼。** 遊戲以 UTF-8 儲存並顯示文字，包括選單、提示框、事件和地圖。為舊補丁製作的中文化包（帶有它的跳脫編碼）會在載入時轉換為 UTF-8，無需修改即可使用。
- **換行。** 中文和日文沒有空格，遊戲原本找不到換行的位置（遇到很長的段落甚至會閃退）。Mei 依照 Unicode 的規則在字與字之間換行，逗號和句號不會出現在行首。
- **字型。** 中文化 Mod 自帶的字型照常使用。遊戲自帶的字型沒有中日韓文字，凡是遊戲要用這些字型的地方，Mei 會換上以思源黑體（Source Han Sans）製作的字型（約 13,000 個字：簡體和繁體中文、日文假名和漢字、韓文諺文）。
- **搜尋。** 外交國家列表和省份尋找支援漢字、全拼、首字母和混合輸入，較長的拼音允許一個錯字。
- **輸入。** 鍵盤輸入或透過輸入法送出的文字會以 UTF-8 進入文字框，退格、刪除和方向鍵按字移動。

![](https://i.imgur.com/Uf8Z21n.png)

## Mei 不是中文化
你還需要一個中文化 Mod。我們用以下 Mod 測試過 Mei（每次只開 Mei 和該 Mod，使用預設設定）：
- 簡體中文：[Chinese Language Mod for 1.37](https://steamcommunity.com/sharedfiles/filedetails/?id=2976470733)
- 繁體中文：[Traditional Chinese for 1.37.5](https://steamcommunity.com/sharedfiles/filedetails/?id=2437805432)
- 韓文：[Korean Language Mod - stability](https://steamcommunity.com/sharedfiles/filedetails/?id=2714934430)

工作坊上沒有找到適用於 1.37 的日文翻譯；任何包含日文的 Mod，Mei 都能顯示其文字。

![](https://i.imgur.com/Uf8Z21n.png)

## 訂閱之前
- 遊戲資料夾中必須安裝 **[Anvil](https://anvil.happydwarf.dev/) 1.8 或更新版本**。請在 [anvil.happydwarf.dev](https://anvil.happydwarf.dev/) 點擊 Download 下載。工作坊無法替你安裝它。
- **EU4 1.37.5**（Steam，Windows）。
- **請先移除舊的雙字節補丁**（它放進遊戲資料夾的 `version.dll`、`d3d9.dll` 和 `plugins` 資料夾）。它和 Anvil 都使用 `version.dll`，兩者只能保留一個。

![](https://i.imgur.com/Uf8Z21n.png)

## 開始使用
1. 訂閱 Mei 和你的中文化 Mod，並在播放集中同時啟用。
2. 啟動遊戲。Anvil 會提示有新外掛等待核准：按 **Ctrl+Shift+F12**，開啟 **Mods** 並核准 Mei。也可以在開始遊戲前在 Anvil Manager 中核准。
3. 重新啟動遊戲。無需任何設定：設定（**Ctrl+Shift+F12**，Settings）已經為中文化 Mod 調好。

![](https://i.imgur.com/Uf8Z21n.png)

## 尚未完成
- **輸入法候選視窗：** 輸入法只測試過已送出的文字；候選視窗和正在組字的內容尚未測試。
- **選取文字和復原** 在文字框中尚未完成；輸入、退格、刪除和方向鍵已經可用。
- **腳本檔案和存檔** 尚未轉換，因此把名稱放在腳本中的中文化包（例如帶有動態地名的補充包）會顯示錯誤的名稱。請只使用基礎中文化包。
- Mei 自帶字型中的**日文漢字**採用簡體中文字形（思源黑體 SC）。
- 疊加層本身的面板（Anvil 的設定視窗）只顯示拉丁字母。

![](https://i.imgur.com/Uf8Z21n.png)

## 為什麼叫「Mei」
銘（日文讀作 *mei*）是刀匠刻在刀莖上的署名。同一個字在中文裡讀作 *míng*，在韓文裡是 명（*myeong*），意思都是「銘文」：中日韓三種語言共用這一個字，正如 Mei 服務於這三種語言。我們團隊的名字都來自鐵匠鋪。

![](https://i.imgur.com/Uf8Z21n.png)

## 致謝與授權
Mei 移植自 VulonLok（YozoraTempest）的 [EU4 Unicode Patch](https://github.com/YozoraTempest/EU4-Unicode-Patch)（MIT 授權）：文字、搜尋和輸入的邏輯都來自原作者，從其原始碼原樣建置。衷心感謝！Mei 自己的程式碼同樣採用 MIT 授權。字型：Adobe 的思源黑體（Source Han Sans），SIL 開放字型授權 1.1。本專案與 Paradox Interactive 無關。

![](https://i.imgur.com/Uf8Z21n.png)

## Happy Dwarf Team 的其他作品
- **[Anvil](https://anvil.happydwarf.dev/)**：讓 Mei 這樣的外掛進入 EU4 的免費載入器，附帶用於核准和更新外掛的 Manager。
- **[Bellows](https://steamcommunity.com/sharedfiles/filedetails/?id=3823747170)**：遊戲啟動時間從約 39 秒縮短到約 12 秒。
- **[Tongs](https://steamcommunity.com/sharedfiles/filedetails/?id=3826695093)**：面向 Mod 測試者和工具開發者，讓腳本和 AI 代理看到遊戲並操作按鈕。
- **[Etch](https://etch.happydwarf.dev/)**：面向介面 Mod 作者，在 VS Code 中按遊戲的樣子預覽 `.gui` 視窗。
- 我們的全部作品：[happydwarf.dev](https://happydwarf.dev/)

![](https://i.imgur.com/Uf8Z21n.png)

### 其他下載方式
[GitHub](https://github.com/happy-dwarf-team/mei)
