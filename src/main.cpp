#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

const int SIZE = 9;

// This is to read a sudoku board from input
void readSudoku(vector<vector<int>> &matrix) 
{
    cout << "Enter the Sudoku matrix row by row (use 0 for empty cells):\n";
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            cin >> matrix[i][j];
        }
    }
}

// This is to read a sudoku board in from a file
void readSudokuFromFile(vector<vector<int>> &matrix, const string& filename) 
{
    ifstream file(filename);
    if (!file) 
    {
        cout << "Error opening file!\n";
        return;
    }

    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            file >> matrix[i][j];
        }
    }
    file.close();
}


void printSudoku(vector<vector<int>> &matrix) 
{
    cout << "Sudoku Board:\n";
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

bool isSafeMove(vector<vector<int>> &matrix, int row, int col, int currNum)
{
    // Is the current number in the column?
    for (int i = 0; i < SIZE; i++)
    {
        if (matrix[row][i] == currNum)
            return false;
    }

    // Is the current number in the row?
    for (int i = 0; i < SIZE; i++)
    {
        if (matrix[i][col] == currNum)
            return false;
    }

    // Is the current number if the nearest 3x3 matrix
    int subRow = row - (row % 3);
    int subCol = col - (col % 3);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (matrix[subRow + i][subCol + j] == currNum)
                return false;
        }
    }

    return true;
}

bool solveSudokuSequentialBackTracking(vector<vector<int>> &matrix, int row, int col)
{
    // Base Case
    // We generally implement backtracking using recursion. Your base case generally checks whether you've succeeded at finding a solution to the problem. If so, it might perform some action (like printing a chess board with n queens on it), and/or it might return some value (a boolean indicating success or a value associated with the solution you just found). Either way, the base case gives your algorithm the ability to return to the previous function call.
    if (row == SIZE - 1 && col == SIZE)
        return true;

    if (col == SIZE)
    {
        row++;
        col = 0;
        solveSudokuSequentialBackTracking(matrix, row, col);
    }
    // (Optional) Check for Duplicate States
    // Sometimes (as with the maze backtracking program I showed in class), you want to explicitly check whether you're in a state that you've already seen. If so, return from this function call rather than allowing yourself to get stuck in an infinite loop where you constantly bounce back and forth between states you've already seen. (This can be thought of as a base case as well.)

    // Sometimes, instead of checking for duplicate states explicitly, we just set up the for-loop that makes our recursive calls in a way that ensures only new, unique states are passed to the function with each recursive call. (That was our approach with the n-queens solution.) Hence, this step that checks for duplicate states is optional.
    // Check if number is not zero - implying that it is already in use
    if (matrix[row][col] != 0)
    {
        col++;
        return solveSudokuSequentialBackTracking(matrix, row, col);
    }

    // Generate Possible Moves
    // This is generally implemented as a for-loop that changes the parameters passed to this function in some way in order to generate all possible states you could reach from your current state. The steps involved with this are:

    // Check Validity of Move
    // In this for-loop, if you're attempting to make some sort of move (i.e., moving a character in a maze), you might first want to check whether it's valid (i.e., does it send your character into a wall, or a fiery pit of doom, or out-of-bounds in your maze array?). If so, skip to the next iteration of this for-loop.

    // Change State
    // Whatever move you're about to make, now's your time to make it. Change the parameters you're passing to your backtracking algorithm to reflect the new state you'll be in after you make this move.

    // Perform Recursive Descent
    // Make your recursive call to the backtracking algorithm, passing it the new state you're in with this move (i.e., passing it the parameters you just modified in order to make the current move).

    for (int i = 1; i <= SIZE; i++)
    {
        if (isSafeMove(matrix, row, col, i))
        {
            matrix[row][col] = i;
            if (solveSudokuSequentialBackTracking(matrix, row, col + 1))
                return true;
            matrix[row][col] = 0;
        }
    }
    return false;
    // (Optional) Process the Return Value of the Recursive Call
    // When your recursive function call returns, you might want to capture its return value. It might be returning a boolean indicating whether it found a path to the solution you want. If it did, you might be able to stop making recursive calls. (That was our first approach to today's n-queens problem.) Alternatively, if you're looking for every possible solution to this problem, you might continue searching even if this recursive call already found one solution. Recall that with n-queens, we modified our function to return how many solutions it had found so far. We captured that return value in our for-loop and added it to a running count of solutions to be returned from the function later.

    // (Optional) Undo State Change
    // If you return from a recursive call, you need to undo the state change you did in step (b) above before you can go on to the next iteration of this move-generating loop. (We saw this is optional in some cases, as with n-queens, where we didn't need to worry about setting whereThemQueensAt[column] to a dummy value because we were about to overwrite it in the next iteration of our for-loop anyway.)
}

void solveSudoku(vector<vector<int>> &matrix) 
{
      solveSudokuSequentialBackTracking(matrix, 0, 0);
}

int main() {
    vector<vector<int>> matrix(SIZE, vector<int>(SIZE, 0));
    readSudokuFromFile(matrix, "../input_files/input1.txt");
    printSudoku(matrix);

    cout << "SOLVING MATRIX USING BACKTRACKING\n";
    solveSudoku(matrix);
    printSudoku(matrix);
    return 0;
}
