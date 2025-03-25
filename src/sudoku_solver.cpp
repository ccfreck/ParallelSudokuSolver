#include "sudoku_solver.h"
#include "timer.h"

double start;

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

void solveSudoku(vector<vector<int>> &matrix) 
{
    solveSudokuSequentialBackTracking(matrix, 0, 0);
}


bool findZero(const SudokuGrid &grid, int &row, int &col)
{
    for (row = 0; row < SIZE; row++)
        for (col = 0; col < SIZE; col++)
            if (grid[row][col] == 0)
                return true;
    return false;
}


bool solve(SudokuGrid grid, int level) 
{
    int row, col;
    if (!findZero(grid, row, col)) 
        return true;

    for (int num = 1; num <= SIZE; num++) {
        if (isSafeMove(grid, row, col, num)) 
        {
            #pragma omp task default(none) firstprivate(grid, row, col, num, level) shared(start, std::cout) final(level > 1)
            {
                SudokuGrid copyGrid = grid;
                copyGrid[row][col] = num;
                if (solve(copyGrid, level + 1)) 
                {
                    printSudoku(copyGrid);
                    //double end = omp_get_wtime();
                    //std::cout << "\nSolved in " << (end - start) << " s\n" << std::endl;
                    //exit(0);
                }
            }
        }
    }
    #pragma omp taskwait
    return false;
}

void parallelDFSGateway(SudokuGrid sudoku)
{
    #pragma omp parallel default(none) shared(sudoku) num_threads(4)
    #pragma omp single nowait
    {
        solve(sudoku, 1);
    }
}

