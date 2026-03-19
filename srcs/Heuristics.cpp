#include "Heuristics.hpp"
#include <cmath>
#include <stdexcept>

namespace Heuristics {

// 曼哈頓距離：每個數字到目標位置的水平+垂直距離之和
int manhattan(const Puzzle& state, const std::vector<int>& goalPos, int size) {
    int dist = 0;
    int total = size * size;

    for (int i = 0; i < total; i++) {
        int val = state.board[i];   // 當前位置
        if (val == 0) continue;     // 跳過空白格(0)
        int gp = goalPos[val];      // 目標位置
        dist += std::abs(i / size - gp / size) + std::abs(i % size - gp % size);
    }
    return dist;
}

// 漢明距離：不在正確位置的數字個數
int hamming(const Puzzle& state, const std::vector<int>& goalPos, int size) {
    int count = 0;
    int total = size * size;

    for (int i = 0; i < total; i++) {
        int val = state.board[i];

        if (val == 0) continue;
        if (goalPos[val] != i) count++;
    }
    return count;
}

// 線性衝突：曼哈頓距離 + 同行/列中順序顛倒的數字對各加 2
int linearConflict(const Puzzle& state, const std::vector<int>& goalPos, int size) {
    int dist = manhattan(state, goalPos, size);
    int conflicts = 0;

    // 檢查每一行的衝突
    for (int row = 0; row < size; row++) {
        for (int i = 0; i < size; i++) {
            int val1 = state.board[row * size + i];
            if (val1 == 0) continue;
            // 如果 val1 的「 目標位置」不在這一列，它就不會造成這一列的線性衝突。
            if (goalPos[val1] / size != row) continue;
            for (int j = i + 1; j < size; j++) {    // 右邊的所有數字
                int val2 = state.board[row * size + j];
                if (val2 == 0) continue;
                // 若 val2 的「目標位置」也不在這一列，跳過。
                if (goalPos[val2] / size != row) continue;

                // 兩者都在目標列中，但相對順序 (Relative Order)跟目標狀態不符。
                if (goalPos[val1] % size > goalPos[val2] % size)
                    conflicts++;
            }
        }
    }

    // 檢查每一列的衝突
    for (int col = 0; col < size; col++) {
        for (int i = 0; i < size; i++) {
            int val1 = state.board[i * size + col];
            if (val1 == 0) continue;
            if (goalPos[val1] % size != col) continue; // val1 目標不在此行

            for (int j = i + 1; j < size; j++) {
                int val2 = state.board[j * size + col];
                if (val2 == 0) continue;
                if (goalPos[val2] % size != col) continue;  // val2 目標不在此行

                // 兩者都在目標行中，但相對順序 (Relative Order)跟目標狀態不符。
                if (goalPos[val1] / size > goalPos[val2] / size)
                    conflicts++;
            }
        }
    }

    return dist + 2 * conflicts;
}
// 為了調轉順序所做的讓路，即「滑出」、「滑回」移動，所以為每個衝突額外加上 2 步的懲罰分數。

HeuristicFunc getHeuristic(const std::string& name) {
    if (name == "manhattan") return manhattan;
    if (name == "hamming") return hamming;
    if (name == "linear_conflict") return linearConflict;
    throw std::runtime_error("Error: Unknown heuristic '" + name + "'. "
                            "Available: manhattan, hamming, linear_conflict.");
}

} // namespace Heuristics
