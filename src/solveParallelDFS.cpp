#include <iostream>
#include <vector>
#include <cstring>
#include <omp.h>
#include <algorithm>

using namespace std;

#define SIZE 9
#define UNASSIGNED 0
double start;

typedef vector<vector<int>> SudokuGrid;

void print_grid(const SudokuGrid &grid) {
    for (const auto &row : grid) {
        for (int num : row) {
            cout << num << " ";
        }
        cout << "\n";
    }
}

bool is_exist_row(const SudokuGrid &grid, int row, int num) {
    return any_of(grid[row].begin(), grid[row].end(), [num](int cell) { return cell == num; });
}

bool is_exist_col(const SudokuGrid &grid, int col, int num) {
    for (int row = 0; row < SIZE; row++) {
        if (grid[row][col] == num) {
            return true;
        }
    }
    return false;
}

bool is_exist_box(const SudokuGrid &grid, int startRow, int startCol, int num) {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (grid[row + startRow][col + startCol] == num) {
                return true;
            }
        }
    }
    return false;
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

bool find_unassigned(const SudokuGrid &grid, int &row, int &col) {
    for (row = 0; row < SIZE; row++) {
        for (col = 0; col < SIZE; col++) {
            if (grid[row][col] == UNASSIGNED) {
                return true;
            }
        }
    }
    return false;
}

bool solve(SudokuGrid grid, int level) {
    int row, col;
    if (!find_unassigned(grid, row, col)) return true;

    for (int num = 1; num <= SIZE; num++) {
        if (isSafeMove(grid, row, col, num)) {
            #pragma omp task default(none) firstprivate(grid, row, col, num, level) shared(start, std::cout) final(level > 1)
            {
                SudokuGrid copy_grid = grid;
                copy_grid[row][col] = num;
                if (solve(copy_grid, level + 1)) {
                    print_grid(copy_grid);
                    double end = omp_get_wtime();
                    std::cout << "\nSolved in " << (end - start) << " s\n" << std::endl;
                    exit(0);
                }
            }
        }
    }
    #pragma omp taskwait
    return false;
}

int main() {
    SudokuGrid sudoku = {
        {0, 0, 0, 0, 0, 0, 0, 1, 0}, 
        {4, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 5, 0, 4, 0, 7},
        {0, 0, 8, 0, 0, 0, 3, 0, 0},
        {0, 0, 1, 0, 9, 0, 0, 0, 0},
        {3, 0, 0, 4, 0, 0, 2, 0, 0},
        {0, 5, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 8, 0, 6, 0, 0, 0} 
    };

    cout << "Size: " << SIZE << "\n";
    start = omp_get_wtime();
    cout << "Solving Sudoku:\n";
    print_grid(sudoku);
    cout << "---------------------\n";

    #pragma omp parallel default(none) shared(sudoku) num_threads(4)
    #pragma omp single nowait
    {
        solve(sudoku, 1);
    }
}
