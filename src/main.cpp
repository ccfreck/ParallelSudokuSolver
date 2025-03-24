#include "sudoku_solver.h"
#include "timer.h"

// int main() 
// {
//     //timer stuff 
//     Timer timer;

//     vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
//     readSudokuFromFile(matrix, "../input_files/input2.txt");
//     printSudoku(matrix);
    
//     cout << "SOLVING MATRIX USING Sequential DFS\n";
    
//     // Call either the sequential or parallel solver
//     timer.start();
//     // double start = omp_get_wtime();
//     // parallelDFSGateway(matrix);
//     solveSudoku(matrix);
//     auto tts = timer.getElapsedTime();

//     std::cout << "execution time for sequential: " << tts << " ms\n" << endl;

//     printSudoku(matrix);
//     return 0;
// }

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

    start = omp_get_wtime();
    cout << "Solving Sudoku:\n";
    printSudoku(sudoku);
    cout << "---------------------\n";

    parallelDFSGateway(sudoku);
}
