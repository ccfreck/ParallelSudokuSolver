#include "sudoku_solver.h"
#include "timer.h"
// compile instructions:
// g++ -std=c++11 main.cpp sudoku_solver.cpp
// ./a.out
int main() 
{
    //timer stuff 
    Timer timer;

    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    printSudoku(matrix);
    
    cout << "SOLVING MATRIX USING BACKTRACKING\n";
    
    // Call either the sequential or parallel solver
    timer.start();
    solveSudoku(matrix);
    auto tts = timer.getElapsedTime();

    std::cout << "execution time for sequential: " << tts << " ms\n" << endl;
    // parallelBackTrackGateway(matrix);

    printSudoku(matrix);
    return 0;
}
