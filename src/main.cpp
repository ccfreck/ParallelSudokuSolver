#include "sudoku_solver.h"
#include "dancing_links.h"

// compile instructions:
// g++ -std=c++11 -fopenmp main.cpp sudoku_solver.cpp dancing_links.cpp
// ./a.out
int main(int argc, char** argv) 
{
    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    printSudoku(matrix);

    runDLXMPIFromMatrix(matrix);

    vector<vector<set<int>>> candidates;

    DLX dlx(NUM_COLS);
    dlx.buildFromSudoku(matrix);
    auto start = chrono::high_resolution_clock::now();
    if (dlx.search(0)) { cout << "solution found" << endl; }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "DLX Done. Time taken: " << duration.count() << " ms\n";
    dlx.printSolution();

    cout << "Initializing candidates...\n";
    initializeCandidates(matrix, candidates);

    start = chrono::high_resolution_clock::now();
    bool changed;
    do
    {
        changed = false;

        if (eliminateSudokuPossibilities(matrix, candidates)) {
            cout << "Applying Elimination Strategy...\n";
            changed = true;
            // printSudoku(matrix);
            continue;
        }

        if (applyLoneRangerStrategy(matrix, candidates)) {
            cout << "Applying Lone Ranger Strategy...\n";
            changed = true;
            // printSudoku(matrix);
            continue;
        }

        if (applyTwinsStrategy(matrix, candidates)) {
            cout << "Applying Twins Strategy...\n";
            changed = true;
            // printSudoku(matrix);
            continue;
        }

        if (applyTripletStrategy(matrix, candidates)) {
            cout << "Applying Triplets Strategy...\n";
            changed = true;
            // printSudoku(matrix);
            continue;
        }

    } while (changed);  

    cout << "SOLVING MATRIX USING BACKTRACKING\n";
    // Call either the sequential or parallel solver
    solveSudoku(matrix);
    // parallelBackTrackGateway(matrix);
    end = chrono::high_resolution_clock::now();
    duration = end - start;
    cout << "Done. Time taken: " << duration.count() << " ms\n";

    printSudoku(matrix);
    return 0;
}
