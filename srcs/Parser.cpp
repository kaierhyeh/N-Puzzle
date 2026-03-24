#include "Parser.hpp"
#include <fstream>   // std::ifstream，開啟檔案讀取盤面資料
#include <sstream>   // std::stringstream，將文字行轉為數值時，能自動跳過空格與註解
#include <stdexcept> // std::runtime_error，當檔案格式錯誤時能優雅地噴出錯誤訊息
#include <set>       // std::set，搜尋前用來檢查盤面數字是否重複或漏掉（0 到 N^2-1）
#include <random>    // 隨機數產生器，用於 generateRandom 隨機生成可解拼圖
#include <chrono>    // 高精度時間，用來作為隨機數種子，保證每次生成的拼圖都不一樣

namespace Parser {

// 讀取檔案並解析盤面
Puzzle parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Error: Cannot open file '" + filename + "'.");

    int                 size = 0;
    std::vector<int>    tiles;
    bool                sizeRead = false;
    std::string         line;

    while (std::getline(file, line)) {
        // 去掉 # 後的註解
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos)
            line = line.substr(0, commentPos);

        // 跳過空行
        if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;

        std::istringstream iss(line);

        // 第一個非空行應為拼圖大小
        if (!sizeRead) {
            // 從流中取第一個整數並填進 size。若內容不是數字（例如寫成 "Three"）會回傳 false。
            if (!(iss >> size) || size < 2)
                throw std::runtime_error("Error: Invalid puzzle size.");
            int extra;
            if (iss >> extra)   // 檢查 size 後是否還有其他數字。
                throw std::runtime_error("Error: Unexpected data on size line.");
            sizeRead = true;
        } else {
            // 讀取數字
            int num;
            while (iss >> num)
                tiles.push_back(num);
        }
    }

    if (!sizeRead)
        throw std::runtime_error("Error: No puzzle size found in file.");

    int total = size * size;
    if ((int)tiles.size() != total)
        throw std::runtime_error("Error: Expected " + std::to_string(total)
                                + " tiles, got " + std::to_string(tiles.size()) + ".");

    // 驗證: 必須包含 0 到 N*N-1 的所有數字（set 會自動排序並除去重複的數字）
    std::set<int> seen(tiles.begin(), tiles.end());
    if ((int)seen.size() != total)
        throw std::runtime_error("Error: Duplicate tile values found.");
    if (*seen.begin() != 0 || *seen.rbegin() != total - 1)
        throw std::runtime_error("Error: Tile values must be in range [0, "
                                + std::to_string(total - 1) + "].");

    return Puzzle(size, tiles);
}

// 從目標狀態出發做隨機移動，保證生成的拼圖一定可解
Puzzle generateRandom(int n) {
    Puzzle          current = Puzzle::generateGoal(n);
    std::mt19937    rng(std::chrono::steady_clock::now().time_since_epoch().count());
    int             moves = n * n * n * 100;

    for (int i = 0; i < moves; i++) {
        auto neighbors = current.getNeighbors();    // getNeighbors 往上下左右滑動後的盤面的清單
        std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
        current = neighbors[dist(rng)];
    }
    return current;
}
// Mersenne twister: a pseudorandom number generator
//     隨機性極高（不被肉眼看穿），但數字可能非常大。
// std::uniform_int_distribution<int> dist(a, b):
//     於 [a, b] 的均勻整數分布。必須給一個數字產生器，回傳[a, b]內的整數。(含頭尾，所以是[])
//     dist 會把 rng 的範圍映射到[a, b]。
//         假設範圍是 0 ~ Max，目標範圍大小是 N (例如 0, 1, 2, 3， N=4)。
//         dist會用除法將 rng 的範圍映射到[a, b]。
//         若 rng 吐出的原始大數字「剛好掉在那個除不盡的尾巴上」時，就會被拒絕，以確保公平性。
//         (因若用 rand() % N，會因餘數而產生偏誤)


// Mersenne Twister: a pseudorandom number generator
//     Produces highly random-looking numbers (hard to detect patterns),
//     but the generated values can be very large.
//
// std::uniform_int_distribution<int> dist(a, b):
//     A uniform integer distribution over [a, b] (inclusive).
//     It requires a random number generator and returns an integer within [a, b].
//     The distribution maps the generator's output range into [a, b].
//
//     Suppose the generator produces values in the range [0, Max],
//     and the target range size is N (e.g., {0, 1, 2, 3}, N = 4).
//     The distribution uses division to map the generator’s range into [a, b].
//
//     If a generated number falls into the "leftover tail" (the part that cannot be evenly divided),
//     it is discarded to ensure fairness (uniformity).
//
//     (In contrast, using rand() % N introduces bias due to uneven remainders.)
} // namespace Parser
