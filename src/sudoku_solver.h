#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <omp.h>

using namespace std;

const int SIZE = 9;
const int UNASSIGNED = 0;

void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename);
void printSudoku(vector<vector<int>> &matrix);
bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum);
bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col);
void parallelBackTrackingSudoku(vector<vector<int>> &matrix, int startNum);
void parallelBackTrackGateway(vector<vector<int>> &matrix);
void solveSudoku(vector<vector<int>> &matrix);
void solveSudokuParallelDFS(vector<vector<int>> &matrix);
bool parallelDFS(vector<vector<int>> &matrix, int row, int col);


bool solve(vector<vector<int>> &grid, int level); 
void solveSudokuParallelGateway(vector<vector<int>> &matrix);

#endif // SUDOKU_SOLVER_H
