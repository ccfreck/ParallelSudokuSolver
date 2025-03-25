#ifndef MULTIPLE_PUZZLES_H
#define MULTIPLE_PUZZLES_H

#include "sudoku_solver.h"
#include "dancing_links.h"
using namespace std;

extern mutex cout_mutex;

class ThreadSafeQueue {
private:
    queue<SudokuGrid> q;
    mutex mtx;
    condition_variable cv;

public:
    void push(const SudokuGrid& grid);
    SudokuGrid pop();
    bool empty();
};

void fillSudokuQueue(ThreadSafeQueue& sudokuQueue, const string& filePath);
void solveFromQueue(ThreadSafeQueue& queue, int userChoice);

#endif // MULTIPLE_PUZZLES_H
