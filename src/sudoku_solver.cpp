#include "sudoku_solver.h"
#include <atomic> // For atomic flag

std::mutex mtxBoardCopies;

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

void printSudoku(const vector<vector<int>> &matrix) 
{
    cout << "Sudoku Board:\n";
    for (const auto& row : matrix) 
    {
        for (int num : row) 
        {
            cout << num << " ";
        }
        cout << endl;
    }
}

bool isSafeMove(const vector<vector<int>> &matrix, int row, int col, int currNum)
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

void parallelBackTrackingSudoku(vector<vector<int>> &board, int startNum, vector<vector<int>> &solvedBoard, std::atomic<bool> &solvedFlag)
{
    if (isSafeMove(board, 0, 0, startNum))
    {
        board[0][0] = startNum;
        if (solveSudokuSequentialBackTracking(board, 0, 1))
        {
            lock_guard<mutex> lock(mtxBoardCopies);
            if (!solvedFlag)
            {
                solvedBoard = board;
                solvedFlag = true;
            }
        }
    }
}



void parallelBackTrackGateway(vector<vector<int>> &matrix)
{
    vector<thread> threads;
    vector<vector<int>> solvedBoard(SIZE, vector<int>(SIZE, 0));
    std::atomic<bool> solvedFlag(false);

    for (int i = 1; i <= SIZE; i++)
    {
        threads.emplace_back([&, i]() {
            vector<vector<int>> boardCopy = matrix;
            parallelBackTrackingSudoku(boardCopy, i, solvedBoard, solvedFlag);
        });
    }

    for (auto &t : threads)
    {
        t.join();
    }

    if (solvedFlag.load())
    {
        matrix = solvedBoard; // Update the original matrix with the solved board
    } else {
        cout << "No solution found.\n";
    }
}


void solveSudoku(vector<vector<int>> &matrix) 
{
    solveSudokuSequentialBackTracking(matrix, 0, 0);
}
