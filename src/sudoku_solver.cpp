#include "sudoku_solver.h"
#include <queue>
#include <condition_variable>
#include <thread>
#include <atomic>

// Define the global control variable
bool useParallelSolver = false;
float countTime;
int numTries;

bool isSafeMoveParallel(vector<vector<int>> &matrix, int row, int col, int currNum) {
    // Atomic flags for each check
    atomic<bool> rowSafe(true), colSafe(true), subgridSafe(true);

    // Lambda function to check a row
    auto checkRow = [&]() {
        for (int i = 0; i < SIZE; i++) {
            if (matrix[row][i] == currNum) {
                rowSafe = false;
                return;
            }
        }
    };

    // Lambda function to check a column
    auto checkCol = [&]() {
        for (int i = 0; i < SIZE; i++) {
            if (matrix[i][col] == currNum) {
                colSafe = false;
                return;
            }
        }
    };

    // Lambda function to check a subgrid
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

    // Launch tasks in parallel
    thread rowThread(checkRow);
    thread colThread(checkCol);
    thread subgridThread(checkSubgrid);

    // Wait for all threads to complete
    rowThread.join();
    colThread.join();
    subgridThread.join();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    //cout << elapsed.count() << endl;

    numTries ++;
    countTime += elapsed.count();
   // cout << "Time spent in thread allocation and execution: " << elapsed.count() << " seconds" << endl;
    // Combine results
    return rowSafe && colSafe && subgridSafe;
}

// Rename the original implementation to Sequential
bool isSafeMoveSequential(vector<vector<int>> &matrix, int row, int col, int currNum)
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

// Create the control function that branches between implementations
bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum)
{
    return useParallelSolver ? 
           isSafeMoveParallel(matrix, row, col, currNum) : 
           isSafeMoveSequential(matrix, row, col, currNum);
}

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
    if(useParallelSolver == true){
        cout << "total time in thread acquisition: " << countTime << endl;
        cout << "          total number of checks: " << numTries << endl;
        cout << "                  avg alloc time: " << countTime/ numTries<< endl;
    }
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
