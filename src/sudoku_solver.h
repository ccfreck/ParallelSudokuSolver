#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <omp.h>
#include <cstring>
#include <algorithm>

using namespace std;

typedef vector<vector<int>> SudokuGrid;

const int SIZE = 9;

extern double start;

void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename);
void printSudoku(vector<vector<int>> &matrix);
bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum);
bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col);
void solveSudoku(vector<vector<int>> &matrix);

bool findZero(const SudokuGrid &grid, int &row, int &col);
void parallelDFSGateway(SudokuGrid sudoku);
bool solve(SudokuGrid grid, int level);

#endif // SUDOKU_SOLVER_H
