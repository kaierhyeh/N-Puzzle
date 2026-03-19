#ifndef PUZZLE_HPP
#define PUZZLE_HPP

#include <functional> // std::hash 模板，底下的 std::hash<Puzzle> 特化需要用到它才能順利編譯
#include <iostream>   // std::cout, std::ostream，讓 Puzzle.print() 函式印出盤面
#include <string>     // C++ 字串的標頭檔，可能被其他包含此檔的地方或函式用來處理字串轉換
#include <vector>     // std::vector<int> board

// std::vector<int>（一維動態陣列）：
//     - 記憶體連續性 (Cache Locality)：一維陣列在記憶體中是完全連續的，會導致 CPU 的 Cache 命中率大幅下降。
//     - 複製成本 (Copy Overhead)：複製一個一維陣列 → 低。
//       （拷貝二維陣列則需要呼叫每一行的 Copy Constructor，這是非常巨大的效能瓶頸）
//     - 經實測，對於 4x4 以上的大盤面，使用 1D 陣列是讓程式能在幾秒鐘內跑完的致勝關鍵。
// ie.
// Row 0:  [8] [1] [3]
// Row 1:  [4] [0] [2]   (0 代表空白格)
// Row 2:  [7] [6] [5]
//     存為一維陣列：[8, 1, 3, 4, 0, 2, 7, 6, 5]，
//     因此在程式裡，移動空白格或計算啟發式函數時，會頻繁看到 / (除法) 和 % (取餘數) 。
class Puzzle {
public:
    std::vector<int> board;
    int size;     // N (board is N×N)
    int zeroPos;  // position of empty cell (0)

    Puzzle();
    Puzzle(int n, const std::vector<int>& tiles);

    // Generate all valid neighbor states (by moving blank)
    std::vector<Puzzle> getNeighbors() const;

    // Generate the snail/spiral goal state for an N×N puzzle
    static Puzzle generateGoal(int n);

    // Check if this puzzle is solvable (relative to given goal)
    bool isSolvable(const Puzzle& goal) const;

    // Pretty print the board
    void print(std::ostream& os = std::cout) const;

    bool operator==(const Puzzle& other) const;
    bool operator!=(const Puzzle& other) const;
};

// Hash specialization for unordered containers
namespace std {            // 打開標準庫的大門
    template<>             // 告訴編譯器：「我要提供一個特製版的模板！」
    struct hash<Puzzle> {  // 當你需要 hash 一個 Puzzle 時，請看這裡
        size_t operator()(const Puzzle& p) const; // 這裡面定義了具體的攪碎算法
    };
}

#endif

// 在 C++ 中，namespace（命名空間） 就像是電腦裡的**「資料夾」**。
// 它的主要用途是為了避免命名衝突 (Naming Collisions)。
// 透過 namespace，我們可以把程式碼分類：
// - Parser::solve（Parser 資料夾裡的搜尋函式）
// - Solver::solve（Solver 資料夾裡的搜尋函式）