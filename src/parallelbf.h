#ifndef PARALLELBF_H
#define PARALLELBF_H

#include <vector>
#include <iostream>
#include <vector>
#include <utility>
#include <omp.h>
#include <cmath>
#include <algorithm>
#include "sudoku_solver.h"

using namespace std;

bool solveParallelBruteForce(std::vector<std::vector<int>> &board);

#endif
