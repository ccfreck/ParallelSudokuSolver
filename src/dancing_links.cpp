#include "dancing_links.h"

void runDLXMPIFromMatrix(const vector<vector<int>>& matrix)
{
    vector<int> flatPuzzle;
    for (const auto& row : matrix)
    {
        for (int val : row)
        {
            flatPuzzle.push_back(val);
        }
    }

    auto start = chrono::high_resolution_clock::now();

    int numThreads = 4;
    bool solutionFound = false;
    DLXMPI* solvedInstance = nullptr;

    #pragma omp parallel for shared(solutionFound, solvedInstance)
    for (int i = 0; i < numThreads; ++i)
    {
        if (!solutionFound)
        {
            DLXMPI* dlx = new DLXMPI(NUM_COLS);
            dlx->buildFromFlatPuzzle(flatPuzzle.data());
            if (dlx->search(0))
            {
                #pragma omp critical
                {
                    if (!solutionFound)
                    {
                        solutionFound = true;
                        solvedInstance = dlx;
                    }
                }
            }
            else
            {
                delete dlx;
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;

    if (solvedInstance)
    {
        cout << "DLXMPI solution found:" << endl;
        solvedInstance->printSolution();
        cout << "DLXMPI Done. Time taken: " << duration.count() << " ms\n";
        delete solvedInstance;
    }
    else
    {
        cout << "No solution found with DLXMPI.\n";
    }
}
