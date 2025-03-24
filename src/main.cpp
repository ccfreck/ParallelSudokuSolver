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
    
    // Prepare matrices for all trials to avoid allocation overhead
    vector<vector<vector<int>>> seqMatrices(NUM_TRIALS, matrix);
    vector<vector<vector<int>>> parMatrices(NUM_TRIALS, matrix);
    
    // Test sequential solver
    cout << "\nRunning " << NUM_TRIALS << " trials with sequential isSafeMove...\n";
    useParallelSolver = false;
    
    timer.start();
    for(int i = 0; i < NUM_TRIALS; i++) {
        solveSudoku(seqMatrices[i]);
    }
    double totalSeqTime = timer.getElapsedTime();
    double avgSeqTime = totalSeqTime / NUM_TRIALS;
    cout << "Average sequential solution time: " << avgSeqTime << " ms\n";
    
    // Test parallel solver
    cout << "\nRunning " << NUM_TRIALS << " trials with parallel isSafeMove...\n";
    useParallelSolver = true;
    
    timer.start();
    for(int i = 0; i < NUM_TRIALS; i++) {
        solveSudoku(parMatrices[i]);
    }
    double totalParTime = timer.getElapsedTime();
    double avgParTime = totalParTime / NUM_TRIALS;
    cout << "Average parallel solution time: " << avgParTime << " ms\n";
    
    cout << "\nAverage speedup: " << avgSeqTime/avgParTime << "x\n";
    
    return 0;
}