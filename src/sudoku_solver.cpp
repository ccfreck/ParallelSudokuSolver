#include "sudoku_solver.h"
#include <queue>
#include <condition_variable>
#include <thread>
#include <atomic>

// Define the global control variable
bool useParallelSolver = false;
float countTime;
int numTries;

// Shared solution tracking for parallel DFS
std::atomic<bool> globalSolutionFound(false);
SudokuGrid globalFinalSolution;

bool isSafeMoveParallel(vector<vector<int>> &matrix, int row, int col, int currNum) {
    std::atomic<bool> rowSafe(true), colSafe(true), subgridSafe(true);

    auto checkRow = [&]() {
        for (int i = 0; i < SIZE; i++) {
            if (matrix[row][i] == currNum) {
                rowSafe = false;
                return;
            }
        }
    };

    auto checkCol = [&]() {
        for (int i = 0; i < SIZE; i++) {
            if (matrix[i][col] == currNum) {
                colSafe = false;
                return;
            }
        }
    };

    auto checkSubgrid = [&]() {
        int subRow = row - (row % 3);
        int subCol = col - (col % 3);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (matrix[subRow + i][subCol + j] == currNum) {
                    subgridSafe = false;
                    return;
                }
            }
        }
    };

    auto start = chrono::high_resolution_clock::now();

    thread rowThread(checkRow);
    thread colThread(checkCol);
    thread subgridThread(checkSubgrid);

    rowThread.join();
    colThread.join();
    subgridThread.join();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    numTries++;
    countTime += elapsed.count();

    return rowSafe && colSafe && subgridSafe;
}

bool isSafeMoveSequential(vector<vector<int>> &matrix, int row, int col, int currNum) {
    for (int i = 0; i < SIZE; i++) {
        if (matrix[row][i] == currNum || matrix[i][col] == currNum)
            return false;
    }

    int subRow = row - (row % 3);
    int subCol = col - (col % 3);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (matrix[subRow + i][subCol + j] == currNum)
                return false;
        }
    }

    return true;
}

bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum) {
    return useParallelSolver ? 
           isSafeMoveParallel(matrix, row, col, currNum) : 
           isSafeMoveSequential(matrix, row, col, currNum);
}

void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error opening file!\n";
        return;
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            file >> matrix[i][j];
        }
    }
    file.close();
}

void printSudoku(const vector<vector<int>>& board) {
    for (int i = 0; i < SIZE; i++) {
        if (i % 3 == 0 && i != 0) cout << "------+-------+------\n";
        for (int j = 0; j < SIZE; j++) {
            if (j % 3 == 0 && j != 0) cout << "| ";
            cout << (board[i][j] == 0 ? "." : to_string(board[i][j])) << " ";
        }
        cout << endl;
    }
}

bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col) {
    if (row == SIZE - 1 && col == SIZE)
        return true;

    if (col == SIZE) {
        row++;
        col = 0;
        return solveSudokuSequentialBackTracking(matrix, row, col);
    }

    if (matrix[row][col] != 0) {
        return solveSudokuSequentialBackTracking(matrix, row, col + 1);
    }

    for (int i = 1; i <= SIZE; i++) {
        if (isSafeMove(matrix, row, col, i)) {
            matrix[row][col] = i;
            if (solveSudokuSequentialBackTracking(matrix, row, col + 1))
                return true;
            matrix[row][col] = 0;
        }
    }
    return false;
}

void solveSudoku(vector<vector<int>> &matrix) {
    solveSudokuSequentialBackTracking(matrix, 0, 0);
    if (useParallelSolver) {
        cout << "total time in thread acquisition: " << countTime << endl;
        cout << "          total number of checks: " << numTries << endl;
        cout << "                  avg alloc time: " << countTime / numTries << endl;
    }
}

bool findZero(const SudokuGrid &grid, int &row, int &col) {
    for (row = 0; row < SIZE; row++)
        for (col = 0; col < SIZE; col++)
            if (grid[row][col] == 0)
                return true;
    return false;
}

bool solve(SudokuGrid grid, SudokuGrid& result, std::atomic<bool>& localSolutionFound) {
    if (localSolutionFound) return false;

    int row, col;
    if (!findZero(grid, row, col)) {
        if (!localSolutionFound.exchange(true)) {
            result = grid;
        }
        return true;
    }

    for (int num = 1; num <= SIZE; num++) {
        if (isSafeMove(grid, row, col, num)) {
            SudokuGrid copyGrid = grid;
            copyGrid[row][col] = num;
            #pragma omp task default(none) firstprivate(copyGrid) shared(localSolutionFound, result)
            {
                if (!localSolutionFound)
                    solve(copyGrid, result, localSolutionFound);
            }
        }
    }

    #pragma omp taskwait
    return false;
}


void initializeCandidates(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates) {
    candidates.assign(SIZE, vector<set<int>>(SIZE));
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (matrix[row][col] == 0) {
                set<int> possible;
                for (int n = 1; n <= SIZE; n++) possible.insert(n);
                for (int i = 0; i < SIZE; i++) {
                    possible.erase(matrix[row][i]);
                    possible.erase(matrix[i][col]);
                }
                int subRow = row - (row % 3);
                int subCol = col - (col % 3);
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        possible.erase(matrix[subRow + i][subCol + j]);
                    }
                }
                candidates[row][col] = possible;
            }
        }
    }
}

bool eliminateSudokuPossibilities(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates) {
    bool changed = false;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (matrix[row][col] == 0 && candidates[row][col].size() == 1) {
                int num = *candidates[row][col].begin();
                matrix[row][col] = num;
                candidates[row][col].clear();
                changed = true;
                for (int i = 0; i < SIZE; i++) {
                    candidates[row][i].erase(num);
                    candidates[i][col].erase(num);
                }
                int subRow = row - (row % 3);
                int subCol = col - (col % 3);
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        candidates[subRow + i][subCol + j].erase(num);
                    }
                }
            }
        }
    }
    return changed;
}

bool applyLoneRangerStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates)
{
    bool changed = false;

    for (int i = 0; i < SIZE; i++)
    {
        map<int, pair<int, int>> rowCounts, colCounts, boxCounts;

        for (int j = 0; j < SIZE; j++)
        {
            if (matrix[i][j] == 0)
            {
                for (set<int>::iterator it = candidates[i][j].begin(); it != candidates[i][j].end(); ++it)
                {
                    if (rowCounts.find(*it) == rowCounts.end())
                        rowCounts[*it] = make_pair(i, j);
                    else
                        rowCounts[*it] = make_pair(-1, -1);
                }
            }
        }

        // Process unique numbers in rows
        for (map<int, pair<int, int>>::iterator it = rowCounts.begin(); it != rowCounts.end(); ++it)
        {
            if (it->second.first != -1)
            {
                matrix[it->second.first][it->second.second] = it->first;
                candidates[it->second.first][it->second.second].clear();
                changed = true;
            }
        }
    }
    return changed;
}

bool applyTwinsStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates)
{
    bool changed = false;

    for (int i = 0; i < SIZE; i++)
    {
        map<set<int>, vector<pair<int, int>>> rowPairs, colPairs;

        for (int j = 0; j < SIZE; j++)
        {
            if (candidates[i][j].size() == 2)
            {
                rowPairs[candidates[i][j]].push_back(make_pair(i, j));
            }
        }

        for (map<set<int>, vector<pair<int, int>>>::iterator it = rowPairs.begin(); it != rowPairs.end(); ++it)
        {
            if (it->second.size() == 2)
            {
                for (int j = 0; j < SIZE; j++)
                {
                    if (candidates[i][j].size() > 2)
                    {
                        for (set<int>::iterator num = it->first.begin(); num != it->first.end(); ++num)
                        {
                            candidates[i][j].erase(*num);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    return changed;
}

bool applyTripletStrategy(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates)
{
    bool changed = false;

    for (int i = 0; i < SIZE; i++)
    {
        map<set<int>, vector<pair<int, int>>> rowTriplets;

        for (int j = 0; j < SIZE; j++)
        {
            if (candidates[i][j].size() == 2 || candidates[i][j].size() == 3)
            {
                rowTriplets[candidates[i][j]].push_back(make_pair(i, j));
            }
        }

        for (map<set<int>, vector<pair<int, int>>>::iterator it = rowTriplets.begin(); it != rowTriplets.end(); ++it)
        {
            if (it->second.size() == 3)
            {
                for (int j = 0; j < SIZE; j++)
                {
                    if (candidates[i][j].size() > 3)
                    {
                        for (set<int>::iterator num = it->first.begin(); num != it->first.end(); ++num)
                        {
                            candidates[i][j].erase(*num);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    return changed;
}

void parallelDFSGateway(SudokuGrid &sudoku) {
    std::atomic<bool> localFound(false);
    SudokuGrid localResult(SIZE, vector<int>(SIZE, 0));

    #pragma omp parallel
    #pragma omp single
    {
        solve(sudoku, localResult, localFound);
    }

    if (localFound) sudoku = localResult;
}
