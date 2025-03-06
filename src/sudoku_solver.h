#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <mutex>
#include <thread>

using namespace std;

const int SIZE = 9;

void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename);
void printSudoku(const vector<vector<int>> &matrix);  // Ensure this is declared
bool isSafeMove(const vector<vector<int>> &matrix, int row, int col, int currNum);  // Ensure this is declared
bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col);
void parallelBackTrackingSudoku(vector<vector<int>> board, int startNum, vector<vector<int>> &solvedBoard, bool &solvedFlag);
void parallelBackTrackGateway(vector<vector<int>> &matrix);
void solveSudoku(vector<vector<int>> &matrix);

#endif
