#include "sudoku_solver.h"

// compile instructions:
// g++ -std=c++11 -fopenmp main.cpp sudoku_solver.cpp
// ./a.out
int main() 
{
    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    printSudoku(matrix);

    vector<vector<set<int>>> candidates;

    cout << "Initializing candidates...\n";
    initializeCandidates(matrix, candidates);

    bool changed;
    do
    {
        changed = false;

        auto start = chrono::high_resolution_clock::now();
        if (eliminateSudokuPossibilities(matrix, candidates)) {
            cout << "Applying Elimination Strategy...\n";
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> duration = end - start;
            cout << "Done. Time taken: " << duration.count() << " ms\n";
            changed = true;
            printSudoku(matrix);
            continue;
        }

        if (applyLoneRangerStrategy(matrix, candidates)) {
            cout << "Applying Lone Ranger Strategy...\n";
            changed = true;
            printSudoku(matrix);
            continue;
        }

        if (applyTwinsStrategy(matrix, candidates)) {
            cout << "Applying Twins Strategy...\n";
            changed = true;
            printSudoku(matrix);
            continue;
        }

        if (applyTripletStrategy(matrix, candidates)) {
            cout << "Applying Triplets Strategy...\n";
            changed = true;
            printSudoku(matrix);
            continue;
        }

    } while (changed);  

    cout << "SOLVING MATRIX USING BACKTRACKING\n";
    // Call either the sequential or parallel solver
    solveSudoku(matrix);
    // parallelBackTrackGateway(matrix);

    printSudoku(matrix);
    return 0;
}
