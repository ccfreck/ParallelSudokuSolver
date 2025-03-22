#include "sudoku_solver.h"
#include "timer.h"

int main() 
{
    //timer stuff 
    Timer timer;

    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    printSudoku(matrix);
    
    cout << "SOLVING MATRIX USING PARALLEL DFS\n";
    
    // Call either the sequential or parallel solver
    timer.start();
    solveSudokuParallelDFS(matrix);
    auto tts = timer.getElapsedTime();

    std::cout << "execution time for sequential: " << tts << " ms\n" << endl;

    printSudoku(matrix);
    return 0;
}
