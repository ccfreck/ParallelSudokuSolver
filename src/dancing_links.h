#ifndef DANCING_LINKS_H
#define DANCING_LINKS_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <climits>

using namespace std;

const int SUDOKU_SIZE = 9;
const int NUM_CONSTRAINTS = 4;
const int NUM_ROWS = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE; // 729
const int NUM_COLS = SUDOKU_SIZE * SUDOKU_SIZE * NUM_CONSTRAINTS; // 324

struct Column;
struct Node;
struct DLX;

#endif // DANCING_LINKS_H
