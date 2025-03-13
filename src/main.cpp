#include "sudoku_solver.h"

// compile instructions:
// g++ -std=c++11 main.cpp sudoku_solver.cpp
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

        cout << "Applying Elimination Strategy...\n";
        if (eliminateSudokuPossibilities(matrix, candidates)) {
            changed = true;
            printSudoku(matrix);
            continue;
        }

        cout << "Applying Lone Ranger Strategy...\n";
        if (applyLoneRangerStrategy(matrix, candidates)) {
            changed = true;
            printSudoku(matrix);
            continue;
        }

        cout << "Applying Twins Strategy...\n";
        if (applyTwinsStrategy(matrix, candidates)) {
            changed = true;
            printSudoku(matrix);
            continue;
        }

        cout << "Applying Triplets Strategy...\n";
        if (applyTripletStrategy(matrix, candidates)) {
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
