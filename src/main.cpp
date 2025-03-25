#include "sudoku_solver.h"
#include "dancing_links.h"
#include "timer.h"
#include "multiple_puzzles.h"

mutex cout_mutex;

int main() {
    int choice = 1;
    int fileChoice;
    SudokuGrid sudoku(SIZE, vector<int>(SIZE, 0));
    SudokuGrid copy = sudoku;

    while (1) {
        // Ask user to select algorithm type
        cout << "Select solving method:\n";
        cout << "1 -> Sequential DFS\n";
        cout << "2 -> Parallel DFS\n";
        cout << "3 -> Early Exit Time \"Optimization\"\n";
        cout << "4 -> Performance Comparison \n";
        cout << "5 -> Dancing Links\n";
        cout << "6 -> Multiple Puzzles\n";
        cout << "9 -> Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        cout << "\n";
        if (choice == 9)
            return 1;

        // Ask user to select input file (1-10)
        cout << "Select input file (1-10): ";
        cin >> fileChoice;

        if (fileChoice < 1 || fileChoice > 10) {
            cout << "Invalid file selection. Please choose between 1 and 10.\n";
            return 1;
        }

        // Construct file path
        string filePath;
        if (choice == 6)
        {
            filePath = "../input_files/inputLong" + to_string(fileChoice) + ".txt";
        }
        else
        {
            filePath = "../input_files/input" + to_string(fileChoice) + ".txt";            
            // Read Sudoku puzzle from file
            readSudokuFromFile(sudoku, filePath);

        }

        if (choice == 1 || choice == 2) {
            cout << "Solving Sudoku:\n";
            printSudoku(sudoku);
            cout << "---------------------\n";

            Timer timer;

            if (choice == 1) {
                cout << "<<<< Using Sequential DFS >>>>\n";
                copy = sudoku;
                timer.start();
                solveSudoku(copy);
            } else if (choice == 2) {
                cout << "<<<< Using Parallel DFS >>>>\n";
                copy = sudoku;
                timer.start();
                parallelDFSGateway(copy);
            }

            auto elapsedTime = timer.getElapsedTime();

            // Display execution time
            if (elapsedTime >= 2000)
                cout << "Execution time: " << elapsedTime / 1000 << " s\n";
            else
                cout << "Execution time: " << elapsedTime << " ms\n\n";

            printSudoku(copy);
            cout << "\n\n";
        }else if (choice == 3) {
            copy = sudoku;
            Timer timer;
           
            readSudokuFromFile(sudoku, "../input_files/input1.txt");
            
            cout << "Original Sudoku Board:\n";
            printSudoku(sudoku);
            
            
            
            // Test sequential solver
            cout << "<<<<  Using Parallel Collision Detection >>>>\n";
            useParallelSolver = true;
            
            timer.start();
            solveSudoku(sudoku);
            auto elapsedTime = timer.getElapsedTime();
            
            
            cout << "Execution time: " << elapsedTime << " ms\n";
            useParallelSolver = false;
        }
        else if (choice == 4) {
            Timer timer;
            int numTrials = 0;
            
            while(numTrials < 1 || numTrials == 134950) {
                if(numTrials < 1)
                    cout <<  "Enter a positive number please!";
                cout << "Enter number of Trials(1-10000): ";
                cin >> numTrials;
                if(numTrials > 10000) {
                    cout<< "That's a lot. Are you sure you want to continue (may take awhile/crash)?\n1 -> yes\n2 -> no\n";
                    cin >> choice;
                    if(choice == 2)
                        numTrials = 134950;
                }

            }

            // Prepare matrices for all trials to avoid allocation overhead
            vector<vector<vector<int>>> seqMatrices(numTrials, sudoku);
            vector<vector<vector<int>>> parMatrices(numTrials, sudoku);

            // Test sequential solver
            cout << "\nRunning " << numTrials << " trials with sequential solver...\n";
            timer.start();
            for (int i = 0; i < numTrials; i++) {
                solveSudoku(seqMatrices[i]);
            }
            double totalSeqTime = timer.getElapsedTime();
            double avgSeqTime = totalSeqTime / numTrials;
            cout << "Average sequential solution time: " << avgSeqTime << " ms\n";

            // Test parallel solver
            cout << "\nRunning " << numTrials << " trials with parallel solver...\n";
            timer.start();
            for (int i = 0; i < numTrials; i++) {
                parallelDFSGateway(parMatrices[i]);
            }
            double totalParTime = timer.getElapsedTime();
            double avgParTime = totalParTime / numTrials;
            cout << "Average parallel solution time: " << avgParTime << " ms\n";

            // Calculate and display speedup
            cout << "\nAverage speedup: " << avgSeqTime / avgParTime << "x\n\n";
        } else if (choice == 5) {
            cout << "Solving Sudoku:\n";
            printSudoku(sudoku);
            cout << "---------------------\n";

            DLX dlx(NUM_COLS);
            dlx.buildFromSudoku(sudoku);
            cout << "<<<< Solving Sudoku using Dancing Links >>>>\n";
            auto start = chrono::high_resolution_clock::now();
            dlx.search(0);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> duration = end - start;
            cout << "Execution Time: " << duration.count() << " ms\n";
            dlx.printSolution();
            cout << "\n";
        } else if (choice == 6) {
            cout << "Now Solving Multiple Puzzles In Parallel:\n";
            cout << "Select your algorithm:\n";
            cout << "1 -> Sequential DFS\n";
            cout << "2 -> Parallel DFS\n";
            cout << "3 -> Dancing Links\n";

            int userChoice = 0;
            cin >> userChoice;

            if (userChoice < 1 || userChoice > 3) {
                cout << "Invalid choice. Please enter 1, 2, or 3.\n";
                return 1;
            }

            ThreadSafeQueue sudokuQueue;
            fillSudokuQueue(sudokuQueue, filePath);
            while (!sudokuQueue.empty()) {
                Timer timer;
                timer.start();

                vector<thread> threads;
                int numThreads = thread::hardware_concurrency(); // or a fixed number

                for (int i = 0; i < numThreads; ++i) {
                    threads.emplace_back(solveFromQueue, ref(sudokuQueue), userChoice);
                }

                for (auto& t : threads) {
                    t.join();
                }

                double elapsedTime = timer.getElapsedTime();
                cout << "All puzzles solved in " << elapsedTime << " ms.\n";

            }
        } else {
            cout << "Invalid choice. Please enter 1, 2, or 3.\n";
            return 1;
        }
    }
    return 0;
}