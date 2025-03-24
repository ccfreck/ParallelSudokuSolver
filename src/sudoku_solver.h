#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <set>
#include <map>
#include <omp.h>
#include <atomic>

using namespace std;

const int SIZE = 9;

void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename);
void printSudoku(vector<vector<int>> &matrix);
bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum);
bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col);
void parallelBackTrackingSudoku(vector<vector<int>> &matrix, int startNum);
void parallelBackTrackGateway(vector<vector<int>> &matrix);
void initializeCandidates(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
void initializeCandidatesParallel(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool eliminateSudokuPossibilities(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool eliminateSudokuPossibilitiesParallel(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool applyLoneRangerStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool applyTwinsStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
bool applyTripletStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates);
void printCandidates(vector<vector<set<int>>> &candidates);
void solveSudoku(vector<vector<int>> &matrix);

#endif // SUDOKU_SOLVER_H
