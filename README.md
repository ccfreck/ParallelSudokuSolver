# ParallelSudokuSolver
6th March 2025

Compile Instructions:

In the terminal of your choice, after the code repo has been cloned, in the src file please run:
	
	g++ -std=c++11 main.cpp sudoku_solver.cpp
 Then run:
 
 	./a.out



Research Paper Draft (Unfinished)
**ABSTRACT**

Sudoku is a popular number-puzzle game that is often enjoyed in newspapers or mobile apps. The goal of this project aims to parallelize the sequential solutions that solve these Sudoku puzzles. Familiar topics such as multi-threading, conditional variables, and mutexes will be used to solve this problem.

**I. GOALS**
1. Parallelize the sequential backtracking solution
2. Capture the time and percentage that the algorithm surpasses the sequential algorithm in speed
3. Four threads working on copies of the same board to see which start position will find the solution the fastest

**CHALLENGES:**
1. Manipulating a piece in the board interrupts the recursion starting from the beginning piece of the board (aka the algorithm sees the manipulated value as true, therefore, it will display solution not found)
2. Multiple threads working on different parts of the board can result in increased overhead if one thread alters the board before the other thread can check to see if its a valid solution on its current piece (cell)

**Pseudocode for Sequential Backtracking Soln:**

Base Cases:

	if (in final cell of board)

    	return true
	if (in last column)
	    go to next row, col = 0
	if (board[row][col] != 0)
	    return solveSudokuSequentialBacktracking(board, row, column)
	
	make state change:
	for (i in 1 - 9 possible numbers)
	    if (isSafeMove)
	      board[row][column] = i;
	      if (solveSequentialBacktracking)
	          return true;
	      undo state change: board[row][column] = 0
	
	return false
  

**II. Possible Algorithms to use for this project.**

- Dancing Links
- Parallelized Backtracking
- etc... 

**III. Activity Diagram**

- paste picture here


**IV. Time Diagrams of Speedup**

- Ideally a line graph


