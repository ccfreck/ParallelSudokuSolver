#include "sudoku_solver.h"

void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename) 
{
    ifstream file(filename);
    if (!file) 
    {
        cout << "Error opening file!\n";
        return;
    }

    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            file >> matrix[i][j];
        }
    }
    file.close();
}

void printSudoku(vector<vector<int>> &matrix) 
{
    cout << "Sudoku Board:\n";
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum)
{
    for (int i = 0; i < SIZE; i++)
    {
        if (matrix[row][i] == currNum || matrix[i][col] == currNum)
            return false;
    }

    int subRow = row - (row % 3);
    int subCol = col - (col % 3);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (matrix[subRow + i][subCol + j] == currNum)
                return false;
        }
    }

    return true;
}

bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col)
{
    if (row == SIZE - 1 && col == SIZE)
        return true;

    if (col == SIZE)
    {
        row++;
        col = 0;
        return solveSudokuSequentialBackTracking(matrix, row, col);
    }

    if (matrix[row][col] != 0)
    {
        return solveSudokuSequentialBackTracking(matrix, row, col + 1);
    }

    for (int i = 1; i <= SIZE; i++)
    {
        if (isSafeMove(matrix, row, col, i))
        {
            matrix[row][col] = i;
            if (solveSudokuSequentialBackTracking(matrix, row, col + 1))
                return true;
            matrix[row][col] = 0;
        }
    }
    return false;
}

void initializeCandidates(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates) 
{
    candidates.assign(SIZE, vector<set<int>>(SIZE));

    for (int row = 0; row < SIZE; row++) 
    {
        for (int col = 0; col < SIZE; col++) 
        {
            if (matrix[row][col] == 0) 
            {
                set<int> possibleNumbers;
                for (int num = 1; num <= SIZE; num++)
                {
                    possibleNumbers.insert(num);
                }

                // Remove numbers from row & column
                for (int i = 0; i < SIZE; i++)
                {
                    possibleNumbers.erase(matrix[row][i]);
                    possibleNumbers.erase(matrix[i][col]);
                }

                // Remove numbers from 3x3 box
                int subRow = row - (row % 3);
                int subCol = col - (col % 3);
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        possibleNumbers.erase(matrix[subRow + i][subCol + j]);
                    }
                }

                candidates[row][col] = possibleNumbers;
            }
        }
    }
}

void initializeCandidatesParallel(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates) 
{
    candidates.assign(SIZE, vector<set<int>>(SIZE));

    #pragma omp parallel for
    for (int row = 0; row < SIZE; row++) 
    {
        for (int col = 0; col < SIZE; col++) 
        {
            if (matrix[row][col] == 0) 
            {
                set<int> possibleNumbers;
                for (int num = 1; num <= SIZE; num++)
                {
                    possibleNumbers.insert(num);
                }

                // Remove numbers from row & column
                for (int i = 0; i < SIZE; i++)
                {
                    possibleNumbers.erase(matrix[row][i]);
                    possibleNumbers.erase(matrix[i][col]);
                }

                // Remove numbers from 3x3 box
                int subRow = row - (row % 3);
                int subCol = col - (col % 3);
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        possibleNumbers.erase(matrix[subRow + i][subCol + j]);
                    }
                }

                // Critical section needed here to avoid data race on shared matrix
                candidates[row][col] = possibleNumbers;
            }
            else
            {
                candidates[row][col].clear(); // Optional: zero out pre-filled
            }
        }
    }
}


bool eliminateSudokuPossibilities(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates) 
{
    bool changed = false;

    for (int row = 0; row < SIZE; row++)
    {
        for (int col = 0; col < SIZE; col++)
        {
            if (matrix[row][col] == 0 && candidates[row][col].size() == 1) 
            {
                // Get the only possible number
                int num = *candidates[row][col].begin();
                matrix[row][col] = num;
                candidates[row][col].clear();
                changed = true;

                // Remove from row, column, and box
                for (int i = 0; i < SIZE; i++)
                {
                    candidates[row][i].erase(num);
                    candidates[i][col].erase(num);
                }

                int subRow = row - (row % 3);
                int subCol = col - (col % 3);
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        candidates[subRow + i][subCol + j].erase(num);
                    }
                }
            }
        }
    }
    return changed;
}

#include <atomic>

bool eliminateSudokuPossibilitiesParallel(vector<vector<int>> &matrix, vector<vector<set<int>>> &candidates) 
{
    atomic<bool> changed(false);

    #pragma omp parallel for collapse(2)
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (matrix[row][col] == 0 && candidates[row][col].size() == 1) {
                int num = *candidates[row][col].begin();
                matrix[row][col] = num;
                candidates[row][col].clear();
                changed = true;

                // Update candidates for related cells
                for (int i = 0; i < SIZE; i++) {
                    #pragma omp critical
                    {
                        candidates[row][i].erase(num);
                        candidates[i][col].erase(num);
                    }
                }

                int subRow = row - (row % 3);
                int subCol = col - (col % 3);
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        #pragma omp critical
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

void printCandidates(vector<vector<set<int>>> &candidates) {
    cout << "Candidates:\n";
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (candidates[i][j].empty()) cout << "[ ] ";
            else {
                cout << "[";
                for (set<int>::iterator it = candidates[i][j].begin(); it != candidates[i][j].end(); ++it) {
                    cout << *it;
                }
                cout << "] ";
            }
        }
        cout << endl;
    }
}

void solveSudoku(vector<vector<int>> &matrix) 
{
    solveSudokuSequentialBackTracking(matrix, 0, 0);
}
