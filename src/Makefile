# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -fopenmp -pthread -Wall

# Source files
SRCS = main.cpp sudoku_solver.cpp dancing_links.cpp multiple_puzzles.cpp bruteforce.cpp parallelBF.cpp
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = sudoku_solver

# Default rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
