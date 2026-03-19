#include "Puzzle.hpp"
#include "Parser.hpp"
#include "Heuristics.hpp"
#include "Solver.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <fstream>
#include <stdexcept>

static void printUsage(const char* prog) {
    std::cerr << "Usage: " << prog << " [options]\n"
              << "Options:\n"
              << "  -f <file>        Read puzzle from file.\n"
              << "  -s <size>        Generate random puzzle of size NxN (default: 3).\n"
              << "  -h <heuristic>   manhattan | hamming | linear_conflict (default: manhattan)\n"
              << "  -a <algorithm>   astar | uniform | greedy (default: astar)\n"
              << "  --help           Show this help message.\n";
}

struct Config {
    std::string filename;
    int size = 3;
    std::string heuristicName = "manhattan";
    std::string algorithmName = "astar";
    bool useFile = false;
};

static Config parseArguments(int argc, char** argv) {
    Config config;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            config.filename = argv[++i];
            config.useFile = true;
        } else if (std::strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            config.size = std::atoi(argv[++i]);
            if (config.size < 2)
                throw std::invalid_argument("Error: Size must be >= 2.");
        } else if (std::strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            config.heuristicName = argv[++i];
        } else if (std::strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            config.algorithmName = argv[++i];
        } else if (std::strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            std::exit(0);
        } else {
            std::cerr << "Error: Unknown option '" << argv[i] << "'.\n";
            printUsage(argv[0]);
            std::exit(1);
        }
    }
    return config;
}

static Puzzle setupInitialPuzzle(Config& config) {
    Puzzle puzzle;

    if (config.useFile) {
        puzzle = Parser::parseFile(config.filename);
        config.size = puzzle.size;
    } else
        puzzle = Parser::generateRandom(config.size);
    return puzzle;
}

static void saveAndPrintReport(const Config& config, const Puzzle& initial, const Puzzle& goal,
                                const SearchResult& result, double durationSec, long long durationMs) {
    if (!result.solvable) {
        std::cout << "This puzzle is unsolvable.\n";
        std::cout << "Execution time: " << durationSec << " seconds (" << durationMs << " ms)\n";
        return;
    }

    // 準備輸出完整報告至檔案
    std::string outFilename = "solutions/size" + std::to_string(config.size) + "_" + std::to_string(result.moves) + "moves.txt";
    std::ofstream outFile(outFilename);
    if (outFile.is_open()) {
        outFile << "=== N-Puzzle Solver Full Report ===\n";
        outFile << "Size: " << config.size << "x" << config.size << "\n";
        outFile << "Heuristic: " << config.heuristicName << "\n";
        outFile << "Algorithm: " << config.algorithmName << "\n\n";

        outFile << "Initial state:\n";
        initial.print(outFile);
        outFile << "\nGoal state (snail):\n";
        goal.print(outFile);
        outFile << "\n";

        outFile << "Solution found!\n";
        outFile << "Execution time: " << durationSec << " seconds (" << durationMs << " ms)\n";
        outFile << "Number of moves: " << result.moves << "\n";
        outFile << "Time complexity (states opened): " << result.totalOpened << "\n";
        outFile << "Space complexity (max states in memory): " << result.maxStates << "\n\n";

        outFile << "=== Solution Path ===\n";
        for (size_t i = 0; i < result.path.size(); i++) {
            if (i == 0)
                outFile << "--- Initial ---\n";
            else
                outFile << "--- Move " << i << " ---\n";
            result.path[i].print(outFile);
            outFile << "\n";
        }
    }

    // 輸出統計結果至終端機
    std::cout << "Solution found!\n";
    std::cout << "Execution time: " << durationSec << " seconds (" << durationMs << " ms)\n";
    std::cout << "Number of moves: " << result.moves << "\n";
    std::cout << "Time complexity (states opened): " << result.totalOpened << "\n";
    std::cout << "Space complexity (max states in memory): " << result.maxStates << "\n\n";

    if (outFile.is_open()) {
        std::cout << "✅ Full solution report saved to: " << outFilename << "\n\n";
        outFile.close();
    } else {
        std::cerr << "Warning: Could not save solution report to file.\n\n";
    }
}

int main(int argc, char** argv) {
    try {
        // 1. 解析命令列參數
        Config config = parseArguments(argc, argv);

        // 2. 準備起點與終點盤面
        Puzzle initial = setupInitialPuzzle(config);
        Puzzle goal    = Puzzle::generateGoal(config.size);

        // 3. 取得啟發式函數與演算法
        HeuristicFunc heuristic = Heuristics::getHeuristic(config.heuristicName);
        SearchMode    mode      = Solver::getSearchMode(config.algorithmName);

        // 4. 印出初始資訊
        std::cout << "=== N-Puzzle Solver ===\n";
        std::cout << "Size: " << config.size << "x" << config.size << "\n";
        std::cout << "Heuristic: " << config.heuristicName << "\n";
        std::cout << "Algorithm: " << config.algorithmName << "\n\n";

        std::cout << "Initial state:\n";
        initial.print();
        std::cout << "\nGoal state (snail):\n";
        goal.print();
        std::cout << "\n";

        // 5. 開始計時並解題
        auto startTime = std::chrono::high_resolution_clock::now();
        SearchResult result = Solver::solve(initial, goal, heuristic, mode);
        auto endTime = std::chrono::high_resolution_clock::now();

        auto durationSec = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
        auto durationMs  = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        // 6. 輸出結果與結案報告
        saveAndPrintReport(config, initial, goal, result, durationSec, durationMs);

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
