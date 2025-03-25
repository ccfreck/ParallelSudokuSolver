#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include "sudoku_solver.h"
#include <vector>

using namespace std;
using namespace chrono;

bool solveBruteForce(std::vector<std::vector<int>> &board);
bool isBoardValid(const std::vector<std::vector<int>> &board);

#endif
