#include "parallelbf.h"

// Function will check
bool isABoardValid(const vector<vector<int>>& board) 
{
    for (int i = 0; i < SIZE; i++) {
        vector<bool> row(SIZE+1, false), col(SIZE+1, false), box(SIZE+1, false);
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != 0 && row[board[i][j]]) 
                return false;
            row[board[i][j]] = true;

            if (board[j][i] != 0 && col[board[j][i]]) 
                return false;
            col[board[j][i]] = true;

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

void solveChunk(vector<vector<int>> board, const vector<pair<int, int>>& emptyCells, unsigned long long start, unsigned long long end, bool& localSolutionFound, vector<vector<int>>& localSolution) 
{
    unsigned long long current = start;
    for (int i = emptyCells.size() - 1; i >= 0; i--) 
    {
        auto& cell = emptyCells[i];
        board[cell.first][cell.second] = 1 + (current % SIZE);
        current /= SIZE;
    }

    for (unsigned long long count = 0; count < end - start && !localSolutionFound; count++) 
    {
        if (isABoardValid(board)) {
            localSolutionFound = true;
            localSolution = board;
            return;
        }

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
        if (carry > 0) break;
    }
}

bool solveParallelBruteForce(vector<vector<int>>& board) 
{
    vector<vector<set<int>>> candidates;
    initializeCandidates(board, candidates);
    bool changed;
    do
    {
        changed = false;

        if (eliminateSudokuPossibilities(board, candidates)) {
            // cout << "Applying Elimination Strategy...\n";
            changed = true;
            continue;
        }

        if (applyLoneRangerStrategy(board, candidates)) {
            // cout << "Applying Lone Ranger Strategy...\n";
            changed = true;
            continue;
        }

        if (applyTwinsStrategy(board, candidates)) {
            // cout << "Applying Twins Strategy...\n";
            changed = true;
            continue;
        }

        if (applyTripletStrategy(board, candidates)) {
            // cout << "Applying Triplets Strategy...\n";
            changed = true;
            continue;
        }

    } while (changed);  

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

    bool solutionFound = false;
    vector<vector<int>> localSolution(SIZE, vector<int>(SIZE));

    #pragma omp parallel shared(solutionFound, localSolution)
    {
        int thread_num = omp_get_thread_num();
        unsigned long long start = thread_num * chunk_size;
        unsigned long long end = (thread_num == num_threads - 1) ? total_combinations : min(start + chunk_size, total_combinations);

        bool localFound = false;
        vector<vector<int>> threadSolution(SIZE, vector<int>(SIZE));

        solveChunk(board, emptyCells, start, end, localFound, threadSolution);

        #pragma omp critical
        {
            if (localFound && !solutionFound) {
                solutionFound = true;
                localSolution = threadSolution;
            }
        }
    }

    if (solutionFound) 
    {
        board = localSolution;
        return true;
    }
    return false;
}