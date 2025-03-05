#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
using namespace std;

const int SIZE = 9;

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

void parallelBackTrackingSudoku(vector<vector<int>> &matrix, int startNum)
{
    vector<vector<int>> localMatrix = matrix;
    if (isSafeMove(localMatrix, 0, 0, startNum))
    {
        localMatrix[0][0] = startNum;
        solveSudokuSequentialBackTracking(localMatrix, 0, 1);
    }
}

void parallelBackTrackGateway(vector<vector<int>> &matrix)
{
    vector<thread> threads;
    for (int i = 1; i <= SIZE; i++)
    {
        threads.emplace_back(parallelBackTrackingSudoku, ref(matrix), i);
    }

    for (auto &t: threads)
    {
        t.join();
    }
}

void solveSudoku(vector<vector<int>> &matrix) 
{
    solveSudokuSequentialBackTracking(matrix, 0, 0);
}

int main() {
    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    printSudoku(matrix);

    cout << "SOLVING MATRIX USING BACKTRACKING\n";
    
    // Call either the sequential or parallel solver
    // solveSudoku(matrix);
    solveSudoku(matrix);

    printSudoku(matrix);
    return 0;
}
