# N-Puzzle Solver

這是一個以 C++ 撰寫的 N-Puzzle 解析程式。它使用 **A\* (A-Star)** 搜尋演算法，能在短時間內解開各種類型與大小（3x3, 4x4, 5x5...）的拼圖。

此專案旨在深入理解啟發式搜尋 (Heuristic Search) 的運算邏輯與效能優化。

舉例來說，一個被打亂的 8-puzzle (3x3 盤面)：<br/>
<table>
  <tr>
    <td>8</td> <td> </td> <td>6</td>
  </tr>
  <tr>
    <td>5</td> <td>4</td> <td>7</td>
  </tr>
  <tr>
    <td>2</td> <td>3</td> <td>1</td>
  </tr>
</table>

需要透過滑動空白格，最終達成「螺旋形」的目標狀態 (Snail Goal)：<br/>
<table>
  <tr>
    <td>1</td> <td>2</td> <td>3</td>
  </tr>
  <tr>
    <td>8</td> <td> </td> <td>4</td>
  </tr>
  <tr>
    <td>7</td> <td>6</td> <td>5</td>
  </tr>
</table>

## ✨ 特點

- **靈活的大小**：支援從 3x3 到更大尺寸的拼圖。
- **螺旋目標狀態 (Snail Goal Solution)**：解法遵循題目規定的「螺旋狀」最終排列。
- **可解性檢查**：在搜尋前自動判斷拼圖是否有解，避免進入死循環。
- **三種啟發式函數 (Heuristics)**：
  - **Manhattan Distance**: 最經典、標準的距離計算方式。
  - **Hamming Distance**: 計算不在正確位置上的方塊數量。
  - **Linear Conflict**: 以 Manhattan 為基礎，針對同一行列的順序錯誤進行額外懲罰。效能最強大、搜尋效率最高。
- **多種搜尋模式 (Bonus)**：
  - **A\*** (預設): 平衡步數與效能。
  - **Uniform-cost Search**: 僅考慮成本 (g)，保證步數最少（但速度較慢）。
  - **Greedy Best-First Search**: 僅考慮啟發值 (h)，速度最快（但步數可能不是最優）。

## 🚀 安裝與編譯

本專案使用標準 `make` 進行編譯：

```bash
cd N-Puzzle
make
```

## 🛠️ 使用方式

### 基本用法 (隨機生成 3x3 並解題)
```bash
./npuzzle
```

### 指定拼圖檔案
```bash
./npuzzle -f [檔案路徑]
```

### 指定設定
```bash
# 生成 4x4 拼圖並使用最強大的 Linear Conflict 啟發式
./npuzzle -s 4 -h linear_conflict

# 使用不同的演算法
./npuzzle -a greedy
```

### 命令列選項
- `-f <file>`：從指定的檔案讀取拼圖。
- `-s <size>`：生成一個 size × size 的隨機可解拼圖（預設 3x3）。
- `-h <type>`：選擇啟發式：`manhattan`, `hamming`, `linear_conflict`（預設 `manhattan`）。
- `-a <algo>`：選擇演算法：`astar`, `uniform`, `greedy`（預設 `astar`）。

## 📁 檔案格式規範

輸入檔案必須符合以下格式：
```text
# 這是一行註解
3
3 2 6 # 行內註解
1 4 0
8 7 5
```

## 📊 搜尋統計

解題結束後，程式會自動輸出：
- **Execution time**：解題所花費的時間（精確到秒與毫秒）。
- **Time complexity**：一共展開了多少個狀態節點。
- **Space complexity**：記憶體中同時保存的最大節點數。
- **Number of moves**：從起點到終點所需的總步數。
- **Solution Path**：每一步移動的詳細過程將會被存入 `solutions` 資料夾下名為 `sizeN_Mmoves.txt` 的檔案中（N為盤面大小，M為總步數），以保持終端機畫面的整潔。

## 🎨 Web Visualizer (網頁視覺化工具)

根目錄下有獨立視覺化工具 `visualizer.html`。
它**不需要架設任何伺服器**，只需在任何瀏覽器中點開即可使用。

**主要功能：**
1. **Snail Goal (螺旋目標) 支援**：對應螺旋目標，到達正確位置的方塊會發出霓虹光芒。
2. **Interactive Mode (手動點擊)**：可輸入自定義的盤面，手動點擊方塊滑動，隨時查看距離目標的位移數量，適合用來驗證「拼圖奇偶性與連通性」。
3. **Autoplay (自動播放路徑)**：只需將本程式輸出的 `sizeN_Mmoves.txt` 中的解題步驟複製貼上，網頁便會自動演示 A* 演算法計算出的滑動路徑！