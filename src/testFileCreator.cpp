#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <algorithm>

using namespace std;

using SudokuGrid = vector<vector<int>>;

// Check if placing num at (row, col) is valid
bool isSafe(const SudokuGrid &board, int row, int col, int num) {
    for (int x = 0; x < 9; x++) {
        if (board[row][x] == num || board[x][col] == num)
            return false;
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[startRow + i][startCol + j] == num)
                return false;

    return true;
}

// Recursively fills the board
bool fillBoard(SudokuGrid &board, int row = 0, int col = 0) {
    if (row == 9) return true;
    if (col == 9) return fillBoard(board, row + 1, 0);

    vector<int> nums{1,2,3,4,5,6,7,8,9};
    random_shuffle(nums.begin(), nums.end());

    for (int num : nums) {
        if (isSafe(board, row, col, num)) {
            board[row][col] = num;
            if (fillBoard(board, row, col + 1)) return true;
            board[row][col] = 0;
        }
    }
    return false;
}

// Generates a fully solved, valid board
SudokuGrid generateValidBoard() {
    SudokuGrid board(9, vector<int>(9, 0));
    fillBoard(board);
    return board;
}

// Blanks out a number of cells to create a puzzle
void removeCells(SudokuGrid &board, int blanks) {
    int removed = 0;
    while (removed < blanks) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (board[row][col] != 0) {
            board[row][col] = 0;
            removed++;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <number_of_boards>\n";
        return 1;
    }

    int numBoards;
    istringstream ss(argv[1]);
    if (!(ss >> numBoards) || numBoards <= 0) {
        cerr << "Invalid number of boards. Please enter a positive integer.\n";
        return 1;
    }

    srand(static_cast<unsigned>(time(0)));

    ofstream outFile("../input_files/inputLong1.txt");
    if (!outFile) {
        cerr << "Failed to open file for writing.\n";
        return 1;
    }

    outFile << numBoards << "\n";

    for (int b = 0; b < numBoards; ++b) {
        SudokuGrid board = generateValidBoard();
        int blanks = rand() % 51 + 20; // Random between 20 and 50
        removeCells(board, blanks);
        for (const auto& row : board) {
            for (int i = 0; i < 9; ++i) {
                outFile << row[i];
                if (i < 8) outFile << " ";
            }
            outFile << "\n";
        }
        if (b < numBoards - 1)
            outFile << "\n";
    }

    outFile.close();
    cout << "Valid Sudoku puzzles written\n";
    return 0;
}
