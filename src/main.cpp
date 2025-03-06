#include "sudoku_solver.h"

// compile instructions:
// g++ -std=c++11 main.cpp sudoku_solver.cpp
// ./a.out
int main() 
{
    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    printSudoku(matrix);

    cout << "SOLVING MATRIX USING BACKTRACKING\n";
    
    // Call either the sequential or parallel solver
    solveSudoku(matrix);
    // parallelBackTrackGateway(matrix);

    printSudoku(matrix);
    return 0;
}
