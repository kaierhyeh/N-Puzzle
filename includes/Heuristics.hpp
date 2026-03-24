#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

#include "Puzzle.hpp"
#include <functional>
#include <string>
#include <vector>

// 定義 heuristic 格式：(state, goalPositions map, board size)
using HeuristicFunc = std::function<int(const Puzzle&, const std::vector<int>&, int)>;

namespace Heuristics {
    // Sum of Manhattan distances for each tile to its goal position
    int manhattan(const Puzzle& state, const std::vector<int>& goalPos, int size);

    // Number of misplaced tiles (excluding blank)
    int hamming(const Puzzle& state, const std::vector<int>& goalPos, int size);

    // Manhattan + 2 per linear conflict (same row/col, wrong order)
    int linearConflict(const Puzzle& state, const std::vector<int>& goalPos, int size);

    // Get heuristic function by name
    HeuristicFunc getHeuristic(const std::string& name);
}

#endif

/*
 * 啟發式 (Heuristics)：
 * 透過觀察來給每個盤面打一個「預測距離分數」的函數。
 * 這個分數越低，程式就會覺得「這一步看起來離解答比較近」，從而優先走這條路。
 * 這個函數設計越精準，程式嘗試的冤枉路就越少，解析的速度就越快！
 *
 * 在 A* 搜尋演算法中，評分公式為 f(n) = g(n) + h(n)。
 * - g(n) 是「已經花費的真實成本」（從起點走到目前的步數）。
 * - h(n) 就是「啟發式函數」，代表「評估目前狀態到目標狀態的距離」。
 *
 * 一個好的啟發式函數必須是 Admissible（可容許的），意思是它「永遠不能高估」真實成本。
 * 如果它高估了，A* 就無法保證找到最佳解。我們在這裡計算的都是「理論上的最短物理距離下限」。
 */

 /*
 * Heuristics:
 * A function that assigns each puzzle state a "predicted distance score" based on observation.
 * The lower this score, the more the algorithm considers the state to be "closer to the solution,"
 * and thus prioritizes exploring that path.
 * The more accurate this function is, the fewer unnecessary paths the algorithm explores,
 * and the faster the solution is found!
 *
 * In the A* search algorithm, the evaluation function is: f(n) = g(n) + h(n).
 * - g(n) is the "actual cost already incurred" (the number of steps taken from the start).
 * - h(n) is the "heuristic function," representing the estimated distance from the current state to the goal.
 *
 * A good heuristic must be admissible, meaning it must "never overestimate" the true cost.
 * If it does, A* can no longer guarantee finding the optimal solution.
 * The values we compute here represent the theoretical lower bound of the shortest possible distance.
 */