#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "Heuristics.hpp"
#include "Puzzle.hpp"
#include <string>
#include <vector>

enum class SearchMode {
    ASTAR,      // f = g + h
    UNIFORM,    // f = g (uniform-cost search)
    GREEDY      // f = h (greedy best-first search)
};

struct SearchResult {
    bool    solvable;
    int     totalOpened;       // States selected from open set (time complexity)
    int     maxStates;         // Max states in memory simultaneously (size complexity)
    int     moves;             // Number of moves in solution
    std::vector<Puzzle> path;  // Solution path from initial to goal
};

namespace Solver {
    SearchResult solve(const Puzzle& initial, const Puzzle& goal,
                        HeuristicFunc heuristic, SearchMode mode = SearchMode::ASTAR);

    SearchMode getSearchMode(const std::string& name);
}

#endif
