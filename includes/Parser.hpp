#ifndef PARSER_HPP
#define PARSER_HPP

#include "Puzzle.hpp"
#include <string>

namespace Parser {
    // Parse a puzzle from input file
    Puzzle parseFile(const std::string& filename);

    // Generate a random solvable puzzle of size n×n
    Puzzle generateRandom(int n);
}

#endif
