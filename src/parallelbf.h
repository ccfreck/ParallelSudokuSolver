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

extern bool solution_found;
extern std::vector<std::vector<int>> solution;

bool isABoardValid(const vector<vector<int>>& board);
unsigned long long calculateCombination(const vector<pair<int, int>>& emptyCells, const vector<vector<int>>& board);
void solveChunk(vector<vector<int>> board, const vector<pair<int, int>>& emptyCells, unsigned long long start, unsigned long long end, bool& localSolutionFound, vector<vector<int>>& localSolution);
bool solveParallelBruteForce(std::vector<std::vector<int>> &board);

#endif
