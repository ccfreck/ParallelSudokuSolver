#include "parallelbf.h"

bool solution_found = false;
vector<vector<int>> solution(SIZE, vector<int>(SIZE));

// Function will check
bool isABoardValid(const vector<vector<int>>& board) 
{
    // Check rows, columns, and boxes
    for (int i = 0; i < SIZE; i++) {
        vector<bool> row(SIZE+1, false), col(SIZE+1, false), box(SIZE+1, false);
        for (int j = 0; j < SIZE; j++) {
            // Check row
            if (board[i][j] != 0 && row[board[i][j]]) 
                return false;
            row[board[i][j]] = true;
            
            // Check column
            if (board[j][i] != 0 && col[board[j][i]]) 
                return false;
            col[board[j][i]] = true;
            
            // Check subgrid (box)
            int boxRow = 3 * (i / 3) + (j / 3);
            int boxCol = 3 * (i % 3) + (j % 3);
            if (board[boxRow][boxCol] != 0 && box[board[boxRow][boxCol]]) 
                return false;
            box[board[boxRow][boxCol]] = true;
        }
    }
    return true;
}

unsigned long long calculateCombination(const vector<pair<int, int>>& emptyCells, const vector<vector<int>>& board) 
{
    unsigned long long combo = 0;
    for (const auto& cell : emptyCells) {
        combo = combo * SIZE + (board[cell.first][cell.second] - 1);
    }
    return combo;
}

void solveChunk(vector<vector<int>> board, const vector<pair<int, int>>& emptyCells, unsigned long long start, unsigned long long end) 
{
    // Initialize board with the starting combination
    unsigned long long current = start;
    for (int i = emptyCells.size() - 1; i >= 0; i--) 
    {
        auto& cell = emptyCells[i];
        board[cell.first][cell.second] = 1 + (current % SIZE);
        current /= SIZE;
    }

    // Check combinations in this chunk
    for (unsigned long long count = 0; count < end - start && !solution_found; count++) 
    {
        if (isABoardValid(board)) {
            #pragma omp critical
            {
                if (!solution_found) 
                {
                    solution_found = true;
                    solution = board;
                }
            }
            return;
        }

        // Increment to next combination
        int carry = 1;
        for (int i = emptyCells.size() - 1; i >= 0 && carry > 0; i--) 
        {
            auto& cell = emptyCells[i];
            board[cell.first][cell.second] += carry;
            if (board[cell.first][cell.second] > SIZE) 
            {
                board[cell.first][cell.second] = 1;
                carry = 1;
            } 
            else 
            {
                carry = 0;
            }
        }
        if (carry > 0) break; // Overflow means we've checked all combinations
    }
}

bool solveParallelBruteForce(vector<vector<int>>& board) 
{
    vector<pair<int, int>> emptyCells;
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            if (board[i][j] == 0) 
            {
                emptyCells.emplace_back(i, j);
            }
        }
    }

    if (emptyCells.empty()) 
    {
        return isABoardValid(board);
    }

    const unsigned long long total_combinations = static_cast<unsigned long long>(pow(SIZE, emptyCells.size()));
    const int num_threads = omp_get_max_threads();
    const unsigned long long chunk_size = max(1ULL, total_combinations / num_threads);

    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        unsigned long long start = thread_num * chunk_size;
        unsigned long long end = (thread_num == num_threads - 1) ? total_combinations : min(start + chunk_size, total_combinations);
        
        solveChunk(board, emptyCells, start, end);
    }

    if (solution_found) 
    {
        board = solution;
        return true;
    }
    return false;
}