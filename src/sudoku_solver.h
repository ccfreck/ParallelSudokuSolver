#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include "thread_pool.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <omp.h>
#include <cstring>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>
#include <atomic>
#include <set>
#include <chrono>
#include <map>
#include <ctime>
#include <cstdlib>
#include <sstream>

using namespace std;

typedef vector<vector<int>> SudokuGrid;

const int SIZE = 9;
extern bool useParallelSolver;
extern double start;
extern std::atomic<bool> globalSolutionFound;
extern SudokuGrid globalFinalSolution;


void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename);
void printSudoku(const vector<vector<int>>& board);
bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum);
bool isSafeMoveSequential(vector<vector<int>> &matrix, int row, int col, int currNum);
bool isSafeMoveParallel(vector<vector<int>> &matrix, int row, int col, int currNum);
bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col);
void solveSudoku(vector<vector<int>> &matrix);
bool applyTripletStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool applyTwinsStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool applyLoneRangerStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool eliminateSudokuPossibilities(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
void initializeCandidates(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);

bool findZero(const SudokuGrid &grid, int &row, int &col);
void parallelDFSGateway(SudokuGrid& sudoku);
bool solve(SudokuGrid grid, int level);

#endif // SUDOKU_SOLVER_H