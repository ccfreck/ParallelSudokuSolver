#include "multiple_puzzles.h"

void solveMultiplePuzzlesParallel(int userChoice, const string& filePath) {
    ifstream inFile(filePath);
    int numBoards;
    inFile >> numBoards;
    inFile.ignore();

    vector<SudokuGrid> puzzles;
    for (int b = 0; b < numBoards; ++b) {
        SudokuGrid board(9, vector<int>(9));
        for (int i = 0; i < 9; ++i) {
            string line;
            getline(inFile, line);
            istringstream iss(line);
            for (int j = 0; j < 9; ++j) {
                iss >> board[i][j];
            }
        }
        puzzles.push_back(board);
        string blank;
        getline(inFile, blank);
    }

    Timer timer;
    timer.start();

    atomic<int> index(0);
    int numThreads = thread::hardware_concurrency();
    vector<thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&]() {
            while (true) {
                int i = index.fetch_add(1);
                if (i >= puzzles.size()) break;

                SudokuGrid board = puzzles[i];
                if (userChoice == 1) solveSudoku(board);
                else if (userChoice == 2)
                {
                    globalSolutionFound = false;
                    globalFinalSolution = SudokuGrid(SIZE, vector<int>(SIZE, 0));
                    parallelDFSGateway(board);
                }
                else if (userChoice == 3) {
                    DLX dlx(NUM_COLS);
                    dlx.buildFromSudoku(board);
                    dlx.search(0);
                    lock_guard<mutex> lock(cout_mutex);
                    cout << "Dancing Links Solution:\n";
                    dlx.printSolution();
                    cout << "---------------------\n";
                    continue;
                }

                lock_guard<mutex> lock(cout_mutex);
                cout << (userChoice == 1 ? "Sequential DFS Solution:\n" :
                         userChoice == 2 ? "Parallel DFS Solution:\n" :
                         userChoice == 3 ? "Brute Force Solution:\n" :
                                           "Parallel Brute Force Solution:\n");
                printSudoku(board);
                cout << "---------------------\n";
            }
        });
    }

    for (auto& t : threads) t.join();

    double elapsed = timer.getElapsedTime();
    cout << "All puzzles solved in " << elapsed << " ms.\n\n";
}
