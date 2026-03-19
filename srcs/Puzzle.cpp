#include "Puzzle.hpp"
#include <algorithm>  // std::swap，用來在 getNeighbors() 裡交換數字與空白格（0）
#include <iomanip>    // std::setw，用在 print() 中，確保輸出行寬一致、數字對齊
#include <cmath>      // std::log10 算數字的長度，以及 std::abs 算曼哈頓距離的絕對值

Puzzle::Puzzle(): size(0), zeroPos(-1) {}

Puzzle::Puzzle(int n, const std::vector<int>& tiles): board(tiles), size(n) {
    for (int i = 0; i < (int)board.size(); i++) {
        if (board[i] == 0) {
            zeroPos = i;
            break;
        }
    }
}

// 模擬與相鄰點交換後的盤面
std::vector<Puzzle> Puzzle::getNeighbors() const {
    std::vector<Puzzle> neighbors;
    int row = zeroPos / size;
    int col = zeroPos % size;

    // Direction Arrays: 四個元素代表四個方向
    int dr[] = {-1, 1, 0, 0};   // Delta Row：上下移動
    int dc[] = {0, 0, -1, 1};   // Delta Col：左右移動

    for (int d = 0; d < 4; d++) {
        int nr = row + dr[d];
        int nc = col + dc[d];

        if (nr >= 0 && nr < size && nc >= 0 && nc < size) {
            Puzzle neighbor = *this;
            int newPos = nr * size + nc;

            // 進行交換：將空白格與目標位置的數字互換
            std::swap(neighbor.board[zeroPos], neighbor.board[newPos]);
            neighbor.zeroPos = newPos;
            neighbors.push_back(neighbor);  // 放入 neighbors 名單裡
        }
    }
    return neighbors;
}

// 生成蝸牛(螺旋)目標狀態
// 例如 3×3: 1 2 3 / 8 0 4 / 7 6 5
Puzzle Puzzle::generateGoal(int n) {
    std::vector<int> goal(n * n, 0);
    int val = 1;
    int top = 0, bottom = n - 1, left = 0, right = n - 1;
    int total = n * n;

    while (val < total) {
        for (int c = left; c <= right && val < total; c++)      // →
            goal[top * n + c] = val++;
        top++;
        for (int r = top; r <= bottom && val < total; r++)      // ↓
            goal[r * n + right] = val++;
        right--;
        for (int c = right; c >= left && val < total; c--)      // ←
            goal[bottom * n + c] = val++;
        bottom--;
        for (int r = bottom; r >= top && val < total; r--)      // ↑
            goal[r * n + left] = val++;
        left++;
    }
    // 0 (blank) is placed at the remaining unfilled position (spiral center)
    return Puzzle(n, goal);
}

// 可解性檢查：基於完整排列的逆序對 + 空白格曼哈頓距離
bool Puzzle::isSolvable(const Puzzle& goal) const {
    int n = size;
    int total = n * n;

    // goalPos[value] = index in goal *見附錄
    std::vector<int> goalPos(total);
    for (int i = 0; i < total; i++)
        goalPos[goal.board[i]] = i;

    // 建立初始排列（含空白格）
    std::vector<int> perm(total);
    for (int i = 0; i < total; i++)
        perm[i] = goalPos[board[i]];

    // 計算逆序對數量
    int inversions = 0;
    for (int i = 0; i < total; i++)
        for (int j = i + 1; j < total; j++)
            if (perm[i] > perm[j])
                inversions++;

    // 空白格的曼哈頓距離：從「目前位置」到「目標位置」需要走的總步數（不考慮障礙）。
    int blankDist = std::abs(zeroPos / n - goal.zeroPos / n) +
                    std::abs(zeroPos % n - goal.zeroPos % n);

    // 可解 ⟺ (逆序對 + 空白距離) 為偶數
    return (inversions + blankDist) % 2 == 0;
}
// 當你做了一次「合法移動」，發生了什麼事？
// 無論是往上下左右哪一個方向滑動，其實就是**「空白格 (0)」與「某個數字 (X)」互換了位置**。

// 我們分這兩個部分來看：
// -------------------------------------------------------------------
// 第一部分：空白格的曼哈頓距離 (blankDist)
// - 當你移動空白格一格時，它在二維座標上的 Row 或 Col 必定會 +1 或 -1。
// - 也就是說，blankDist 的數值絕對會發生改變，且變化量剛好是 1。
// - 結論：每次移動，blankDist 的奇偶性一定會翻轉（偶數變奇數，或奇數變偶數）。
// -------------------------------------------------------------------
// 第二部分：逆序對數量 (inversions)
// - 既然移動是一次「空白格」與「數字 X」的互換（Swap），那麼整個數列的 inversions 可能會增加或減少。
// - 但關鍵在於：不管它增加或減少了多少個，inversions 的奇偶性也一定會翻轉（例如從 21 變成 22 或 20，總之奇數變偶數）。
// -------------------------------------------------------------------
// 所以不管我們如何移動，盤面的奇偶性(parity)一定會維持不變：逆序對 + 空白距離 必定維持奇數或偶數。
// 所以當盤面與目標盤面的奇偶性不同時，代表這兩個盤面的數學空間不相通。
// 又因解答盤面一定是偶數，所以可解盤面也必定是偶數。

// goalPos: goal.board的反向對照表
//     這個數字該存在哪個位置？
// [1] [2] [3]
// [8] [0] [4]
// [7] [6] [5], goal.board = [1, 2, 3, 8, 0, 4, 7, 6, 5].
// 當 i = 0 時：
//     goal.board[0] 是 1。
//     程式執行：goalPos[1] = 0。
//     解讀：數字 1 的目標位置是在 Index 0。
// 當 i = 4 時：
//     goal.board[4] 是 0。
//     程式執行：goalPos[0] = 4。
//     解讀：數字 0（空白格）的目標位置是在 Index 4。
// 找出每個數字在 std::vector<int> goal.board 中的位置。


void Puzzle::print(std::ostream& os) const {
    int maxVal = size * size - 1;
    int width = (maxVal > 0) ? (int)std::log10(maxVal) + 1: 1;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (j > 0)      // 換下一個數字前，先印一個空白格
                os << " ";
            os << std::setw(width) << board[i * size + j];
        }
        os << "\n";
    }
}
// 不論數字多大，都能讓各數字如表格般對齊。
// std::setw(width)：預留一個寬度為 width 的空間。
//     用來計算為了容納最大的數字所需的空格數。
// std::log10(maxVal) + 1 是一個非常聰明的數學技巧，用來算出數字有幾位數。
// - 例如 3x3 最大數字是 8，log10(8) 大約 0.9，+1 後轉整數 = 1。
// - 例如 4x4 最大數字是 15，log10(15) 大約 1.1，+1 後轉整數 = 2。


bool Puzzle::operator==(const Puzzle& other) const {
    return board == other.board;
}

bool Puzzle::operator!=(const Puzzle& other) const {
    return !(*this == other);
}

namespace std {
    size_t hash<Puzzle>::operator()(const Puzzle& p) const {
        size_t seed = p.board.size();
        for (auto& v: p.board)
            seed ^= hash<int>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
}
// 自定義的雜湊法：賦予 Puzzle 一張唯一的數位身分證，以便快速確認這個盤面有沒有出現過。
// - size_t seed：這是雜湊值的「地基」。
// - 0x9e3779b9：這是一個來自黃金比例的常數（$2^{32} / \phi$），在雜湊界非常有名（通常在 Boost 庫中被廣泛使用）。它的作用是確保雜湊值的每一位元 (bits) 都能夠被隨機且均勻地分散開來，減少碰撞 (Collision)。
// - seed ^= ... + (seed << 6) + (seed >> 2)：這個公式能把盤面上的每一個數字依序「攪拌」進主雜湊值裡面。