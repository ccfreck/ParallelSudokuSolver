#ifndef MULTIPLE_PUZZLES_H
#define MULTIPLE_PUZZLES_H

#include "multiple_puzzles.h"
#include "sudoku_solver.h"
#include "bruteforce.h"
#include "parallelbf.h"
#include "dancing_links.h"
#include "timer.h"
#include <fstream>
#include <sstream>
#include <atomic>
#include <thread>
using namespace std;

extern mutex cout_mutex;

void solveMultiplePuzzlesParallel(int userChoice, const std::string& filePath);


#endif // MULTIPLE_PUZZLES_H
