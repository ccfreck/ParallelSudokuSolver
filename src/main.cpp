#include "sudoku_solver.h"
#include "timer.h"

int main() 
{
    
    const int NUM_TRIALS = 1;
    Timer timer;
    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    
    cout << "Original Sudoku Board:\n";
    printSudoku(matrix);
    
    
    
    // Test sequential solver
    cout << "<<<<  Using Parallel Collision Detection >>>>\n";
    useParallelSolver = true;
    
    timer.start();
    solveSudoku(matrix);
    double elapsedParTime = timer.getElapsedTime();
    
    
    cout << "Parallel solution time: " << elapsedParTime << " ms\n";
    
    return 0;
}