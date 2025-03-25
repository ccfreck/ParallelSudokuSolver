#include "sudoku_solver.h"
#include "timer.h"
#include <iostream>

using namespace std;
// compile instructions:
// g++ -std=c++11 -pthread main.cpp sudoku_solver.cpp -o sudoku_solver
// ./sudoku_solver
int main() {
    int choice = 1;
    int fileChoice;
    SudokuGrid sudoku(SIZE, vector<int>(SIZE, 0));

    while(1)
    {
        // Ask user to select algorithm type
        cout << "Select solving method:\n";
        cout << "1 -> Sequential DFS\n";
        cout << "2 -> Parallel DFS\n";
        cout << "9 -> Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        if(choice == 9)
            return 1;
        // Ask user to select input file (1-10)
        cout << "Select input file (1-10): ";
        cin >> fileChoice;

        if (fileChoice < 1 || fileChoice > 10) {
            cout << "Invalid file selection. Please choose between 1 and 10.\n";
            return 1;
        }

        // Construct file path
        string filePath = "../input_files/input" + to_string(fileChoice) + ".txt";
        
        // Read Sudoku puzzle from file
        readSudokuFromFile(sudoku, filePath);

        cout << "Solving Sudoku:\n";
        printSudoku(sudoku);
        cout << "---------------------\n";

        Timer timer;

        if (choice == 1) 
        {
            cout << "<<<< Using Sequential DFS >>>>\n";
            timer.start();
            solveSudoku(sudoku);
        } 
        else if (choice == 2) 
        {
            cout << "<<<< Using Parallel DFS >>>>\n";
            timer.start();
            parallelDFSGateway(sudoku);
        } 
        else 
        {
            cout << "Invalid choice. Please enter 1 or 2.\n";
            return 1;
        }
         cout << "finsihed" ;
        auto elapsedTime = timer.getElapsedTime();

        //for if it's more than 2 seconds
        if(timer.getElapsedTime() >= 2000)
            cout << "Execution time: " << elapsedTime/1000 << " s\n";
        else
            cout << "Execution time: " << elapsedTime << " ms\n";

        printSudoku(sudoku);
        cout << "\n\n";
    }
    return 0;
}
