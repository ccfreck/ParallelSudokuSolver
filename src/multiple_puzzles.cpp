#include "multiple_puzzles.h"

void ThreadSafeQueue::push(const SudokuGrid& grid) {
    lock_guard<mutex> lock(mtx);
    q.push(grid);
    cv.notify_one();
}

SudokuGrid ThreadSafeQueue::pop() {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [this]() { return !q.empty(); });
    SudokuGrid grid = q.front();
    q.pop();
    return grid;
}

bool ThreadSafeQueue::empty() {
    lock_guard<mutex> lock(mtx);
    return q.empty();
}

void fillSudokuQueue(ThreadSafeQueue& sudokuQueue, const string& filePath) {
    ifstream inFile(filePath);
    if (!inFile) {
        cerr << "Failed to open file: " << filePath << "\n";
        return;
    }

    int numBoards;
    inFile >> numBoards;
    inFile.ignore(); // Skip the newline after the number

    for (int b = 0; b < numBoards; ++b) {
        SudokuGrid board(9, vector<int>(9, 0));
        for (int i = 0; i < 9; ++i) {
            string line;
            getline(inFile, line);
            istringstream iss(line);
            for (int j = 0; j < 9; ++j) {
                iss >> board[i][j];
            }
        }
        sudokuQueue.push(board);
        string emptyLine;
        getline(inFile, emptyLine); // Consume empty line
    }
}

void solveFromQueue(ThreadSafeQueue& queue, int userChoice) {
    while (!queue.empty()) {
        SudokuGrid board = queue.pop();

        if (userChoice == 1) {
            solveSudoku(board);

            lock_guard<mutex> lock(cout_mutex);
            // cout << "Sequential DFS Solution:\n";
            // printSudoku(board);
            // cout << "---------------------\n";

        } else if (userChoice == 2) {
            parallelDFSGateway(board);

            lock_guard<mutex> lock(cout_mutex);
            // cout << "Parallel DFS Solution:\n";
            // printSudoku(board);
            // cout << "---------------------\n";

        } else if (userChoice == 3) {
            DLX dlx(NUM_COLS);
            dlx.buildFromSudoku(board);
            dlx.search(0);

            lock_guard<mutex> lock(cout_mutex);
            // cout << "Dancing Links Solution:\n";
            // dlx.printSolution();
            // cout << "---------------------\n";

        }
    }
}