#include "Solver.hpp"
#include <algorithm>        // std::max, std::reverse
#include <memory>           // std::shared_ptr 管理樹狀結構
#include <queue>            // std::priority_queue
#include <unordered_map>    // 雜湊表
#include <unordered_set>    // 雜湊集合

namespace Solver {

struct Node {       // 不寫在hpp裡是因為外部使用者如 main 不需要也不會用到 Node。
    Puzzle  state;
    int     g;      // cost from start
    int     h;      // heuristic value：越小越接近解答
    int     f;      // priority score：f = g + h
    std::shared_ptr<Node> parent;

    // Min-heap: lower f first; tie-break by lower h (closer to goal)
    // 最小堆積：「數值最小」的那一個永遠會浮上最頂端。
    bool operator>(const Node& other) const {   // ">" 的比較方法。
        if (f != other.f) return f > other.f;   // 規則一：f 越小越優先
        return h > other.h;                     // 規則二：f 相同，h 越小越優先
    }
};

SearchResult solve(const Puzzle& initial, const Puzzle& goal,
                    HeuristicFunc heuristic, SearchMode mode) {
    SearchResult result = {false, 0, 0, 0, {}};

    /* ====================================================================== *
     * 0. 先檢查可解性  Solvability Validation
     * ====================================================================== */
    if (!initial.isSolvable(goal))
        return result;

    /* ====================================================================== *
     * 1. 環境準備與初始化  Initialization & Setup
     * ====================================================================== */
    int n = initial.size;
    int total = n * n;

    // 1-1. 建立目標位置映射: goalPositions[value] = index_in_goal
    std::vector<int> goalPositions(total);
    for (int i = 0; i < total; i++)
        goalPositions[goal.board[i]] = i;

    // 1-2. 建立搜索器
    // 計算 f 值的 Lambda 拋棄式、免洗的快捷函式，僅在 local scope。
    // [&]:以引用（Reference）方式捕獲所有外部變數。這裡只有讀取 "mode"。
    auto calcF = [&](int g, int h) -> int {
        switch (mode) {
            case SearchMode::ASTAR:   return g + h;
            case SearchMode::UNIFORM: return g;
            case SearchMode::GREEDY:  return h;
        }
        return g + h;
    };

    // 1-3. 建立儲存格式
    // - 優先隊列 (min-heap)
    // std::priority_queue：容器適配器 (Container Adapter)
    //     它並不直接管理記憶體，而是封裝了另一個容器（如 vector）並對它施加了 「堆積 (Heap)」 的排序邏輯。
    // std::priority_queue<儲存元素的類型, 存放元素的容器類型, 優先序判定規則>
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openQueue;

    // 已知最佳 g值：Key-Value Pair(Python的dict)，儲存當前的盤面與其對應的 g值，可快速搜尋。
    std::unordered_map<Puzzle, int> bestG;

    // 已處理的盤面：元素有唯一性、沒有順序 (set has unique elements)
    // 以盤面內容直接查詢，而不是用 index: 1, 2, 3...
    std::unordered_set<Puzzle> closed;

    // 用於路徑回溯的節點映射
    // std::shared_ptr：智慧指標 smart pointer (自動回收記憶體，不用 delete)
    std::unordered_map<Puzzle, std::shared_ptr<Node>> nodeMap;

    // 1-4. 初始化起始節點
    auto startNode    = std::make_shared<Node>();   // 建立一個 Node 並回傳其智慧指標 smart pointer，
    startNode->state  = initial;                    // 用 auto 取代很長的 std::shared_ptr<Node>。
    startNode->g      = 0;
    startNode->h      = heuristic(initial, goalPositions, n);
    startNode->f      = calcF(0, startNode->h);
    startNode->parent = nullptr;

    openQueue.push(*startNode);
    bestG[initial] = 0;
    nodeMap[initial] = startNode;

    /* ====================================================================== *
     * 2. 搜索主迴圈  Main Search Loop
     * ====================================================================== */
    while (!openQueue.empty()) {
        result.maxStates = std::max(result.maxStates,
                                    (int)(openQueue.size() + closed.size()));

        // 2-1. 取出目前評分最佳的節點
        Node current = openQueue.top();
        openQueue.pop();

        if (closed.count(current.state))
            continue;   // 跳過已處理的狀態，進入下一輪 while
        closed.insert(current.state);
        result.totalOpened++;

        // 2-2. 目標達成檢查與路徑回溯
        if (current.state == goal) {    // 比較當前 openQueue.top() 的盤面與參數的 goal 是否相同
            result.solvable = true;
            result.moves = current.g;

            // 回溯路徑
            auto node = nodeMap[current.state];
            while (node) {  // 全部存到 result 裡
                result.path.push_back(node->state);
                node = node->parent;
            }
            std::reverse(result.path.begin(), result.path.end());
            return result;
        }

        // 2-3. 展開相鄰節點 (Expand Neighbors)
        for (const auto& neighbor: current.state.getNeighbors()) {
            if (closed.count(neighbor)) continue;

            int newG = current.g + 1;   // g：從起點到這裡的步數

            // 已有更好的路徑則跳過
            if (bestG.count(neighbor) && bestG[neighbor] <= newG) continue;

            bestG[neighbor] = newG;

            // 2-4. 評估與推入新節點
            auto newNode = std::make_shared<Node>();
            newNode->state = neighbor;
            newNode->g = newG;
            newNode->h = heuristic(neighbor, goalPositions, n);
            newNode->f = calcF(newG, newNode->h);
            newNode->parent = nodeMap[current.state];

            nodeMap[neighbor] = newNode;
            openQueue.push(*newNode);
        }
    }

    return result;
}
// Total number of states ever selected in the "opened" set (complexity in time):
// 「程式並沒有一次就走對路徑」。

// 以下用更具體的比喻來解釋它們之間的差異：

// 1. Number of Moves (最終解答的步數) =「正確答案的長度」
//     這就像是用 Google Maps 導航。從出發地到目的地，那條最終畫出來的藍色路線，
//     總共有幾個轉彎（幾步）。在這題，藍色路線就是那 24 步（Move 1 到 Move 24）。

// 2. States Opened (展開的狀態數) =「找路的過程中所嘗試過的交叉路口」
//     你的程式沒有上帝視角，它一開始並不知道哪條路是正確的。

// 在尋找這條 24 步的最佳路徑時，演算法經歷了這樣的過程：

// 1. 程式稍微往左走了一點（展開了一些狀態），覺得分數不錯。
// 2. 走了幾步後，發現原本覺得很好的路，分數越來越差（遇到死胡同或繞遠路了）。
// 3. 於是程式回頭，從記憶體裡挑出另一個一開始沒走、但現在看起來比較有希望的盤面，重新探索另一條分支。
// 4. 這樣來來回回、到處嘗試其他的可能性。
// 在這個過程中，程式總共「嘗試檢查/試探了 774 個不同的盤面」，
// 才終於在茫茫宇宙中，成功拼湊出了那條只需要 24 步就能到達終點的完美路徑。

SearchMode getSearchMode(const std::string& name) {
    if (name == "astar") return SearchMode::ASTAR;
    if (name == "uniform") return SearchMode::UNIFORM;
    if (name == "greedy") return SearchMode::GREEDY;
    throw std::runtime_error("Error: Unknown algorithm '" + name + "'. "
                            "Available: astar, uniform, greedy.");
}

} // namespace Solver
