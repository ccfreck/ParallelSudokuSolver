#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include "thread_pool.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future> // For std::promise and std::future

using namespace std;

const int SIZE = 9;

// Add global control variable
extern bool useParallelSolver;

// Function declarations
void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename);
void printSudoku(vector<vector<int>> &matrix);
bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum);
bool isSafeMoveSequential(vector<vector<int>> &matrix, int row, int col, int currNum);
bool isSafeMoveParallel(vector<vector<int>> &matrix, int row, int col, int currNum);
bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col);
void parallelBackTrackingSudoku(vector<vector<int>> &matrix, int startNum);
void parallelBackTrackGateway(vector<vector<int>> &matrix);
void solveSudoku(vector<vector<int>> &matrix);

#endif // SUDOKU_SOLVER_H