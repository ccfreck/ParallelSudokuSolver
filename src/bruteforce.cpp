#include "bruteforce.h"

// void printSudoku(const vector<vector<int>>& board) 
// {
//     for (int i = 0; i < SIZE; i++) 
//     {
//         if (i % 3 == 0 && i != 0) cout << "------+-------+------\n";
//         for (int j = 0; j < SIZE; j++) 
//         {
//             if (j % 3 == 0 && j != 0) cout << "| ";
//             cout << (board[i][j] == 0 ? "." : to_string(board[i][j])) << " ";
//         }
//         cout << endl;
//     }
// }

bool isBoardValid(const vector<vector<int>>& board) {
    // Check rows, columns, and boxes
    for (int i = 0; i < SIZE; i++) {
        vector<bool> row(SIZE+1, false), col(SIZE+1, false), box(SIZE+1, false);
        for (int j = 0; j < SIZE; j++) {
            // Check row
            if (board[i][j] != 0 && row[board[i][j]]) return false;
            row[board[i][j]] = true;
            
            // Check column
            if (board[j][i] != 0 && col[board[j][i]]) return false;
            col[board[j][i]] = true;
            
            // Check box
            int boxRow = 3 * (i / 3) + (j / 3);
            int boxCol = 3 * (i % 3) + (j % 3);
            if (board[boxRow][boxCol] != 0 && box[board[boxRow][boxCol]]) return false;
            box[board[boxRow][boxCol]] = true;
        }
    }
    return true;
}

bool solveBruteForce(vector<vector<int>>& board) {
    // vector<vector<set<int>>> candidates;
    // initializeCandidates(board, candidates);
    // bool changed;
    // do
    // {
    //     changed = false;

    //     if (eliminateSudokuPossibilities(board, candidates)) {
    //         // cout << "Applying Elimination Strategy...\n";
    //         changed = true;
    //         continue;
    //     }

    //     if (applyLoneRangerStrategy(board, candidates)) {
    //         // cout << "Applying Lone Ranger Strategy...\n";
    //         changed = true;
    //         continue;
    //     }

    //     if (applyTwinsStrategy(board, candidates)) {
    //         // cout << "Applying Twins Strategy...\n";
    //         changed = true;
    //         continue;
    //     }

    //     if (applyTripletStrategy(board, candidates)) {
    //         // cout << "Applying Triplets Strategy...\n";
    //         changed = true;
    //         continue;
    //     }

    // } while (changed);  

    vector<pair<int, int>> emptyCells;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                emptyCells.emplace_back(i, j);
            }
        }
    }
    
    if (emptyCells.empty()) {
        return isBoardValid(board);
    }
    
    // Initialize all empty cells to 1
    for (auto& cell : emptyCells) {
        board[cell.first][cell.second] = 1;
    }
    
    while (true) {
        // Check if current configuration is valid
        if (isBoardValid(board)) {
            return true;
        }
        
        // Increment the "number" represented by empty cells
        int carry = 1;
        for (int i = emptyCells.size() - 1; i >= 0 && carry > 0; i--) {
            auto& cell = emptyCells[i];
            board[cell.first][cell.second] += carry;
            if (board[cell.first][cell.second] > SIZE) {
                board[cell.first][cell.second] = 1;
                carry = 1;
            } else {
                carry = 0;
            }
        }
        
        // If we've overflowed, we've tried all combinations
        if (carry > 0) {
            break;
        }
    }
    
    // Reset board
    for (auto& cell : emptyCells) {
        board[cell.first][cell.second] = 0;
    }
    return false;
}

// int main() {
//     vector<vector<int>> board = {
//         {5, 3, 4, 6, 7, 8, 9, 1, 0},
//         {6, 7, 2, 1, 9, 5, 3, 0, 8},
//         {1, 9, 8, 3, 4, 2, 0, 6, 7},
//         {8, 5, 9, 7, 6, 1, 4, 2, 3},
//         {4, 2, 6, 8, 5, 3, 7, 9, 1},
//         {7, 1, 3, 9, 2, 0, 8, 5, 6},
//         {9, 6, 1, 5, 3, 7, 0, 8, 4},
//         {2, 8, 7, 4, 1, 9, 6, 0, 5},
//         {3, 4, 5, 2, 8, 6, 1, 7, 0}
//     };

//     cout << "Initial board:" << endl;
//     printSudoku(board);
    
//     auto start = high_resolution_clock::now();
//     if (solveBruteForce(board)) {
//         cout << "\nSolution found:" << endl;
//         printSudoku(board);
//     } else {
//         cout << "\nNo solution exists" << endl;
//     }

//     auto stop = high_resolution_clock::now();
//     auto duration = duration_cast<milliseconds>(stop - start);
    
//     // Print the duration to 5 decimal places
//     cout << "\nTime taken: " << fixed << setprecision(5) << duration.count() / 1000.0 << " seconds" << endl;

//     return 0;
// }