#include "dancing_links.h"

// Node in the DLX matrix
struct Node
{
    Node *L, *R, *U, *D;
    Column* C; // Pointer to the column header
    int rowID; // Useful for decoding solution

    Node() : L(this), R(this), U(this), D(this), C(nullptr), rowID(-1) {}
};

// Column header node
struct Column : public Node
{
    int size; // Number of 1s in the column
    string name;

    Column(const string& n) : size(0), name(n)
    {
        C = this;
    }
};

// DLX Root and Matrix
struct DLX
{
    Column* root;
    vector<Column*> columns;
    vector<Node*> nodes;
    vector<Node*> solution;

    DLX(int numCols)
    {
        root = new Column("root");
        columns.resize(numCols);

        Column* prev = root;
        for (int i = 0; i < numCols; i++)
        {
            Column* col = new Column("C" + to_string(i));
            columns[i] = col;

            // Link horizontally
            col->L = prev;
            col->R = root;
            prev->R = col;
            root->L = col;
            prev = col;
        }
    }

    // Create a new row in the matrix (connects 1s to the corresponding columns)
    void addRow(int rowID, const vector<int>& colIndices)
    {
        Node* first = nullptr;
        for (int col : colIndices)
        {
            Column* column = columns[col];
            Node* node = new Node();
            node->C = column;
            node->rowID = rowID;

            // Vertical linking
            node->U = column->U;
            node->D = column;
            column->U->D = node;
            column->U = node;

            column->size++;

            // Horizontal linking
            if (!first)
            {
                first = node;
            }
            node->L = first->L;
            node->R = first;
            first->L->R = node;
            first->L = node;

            nodes.push_back(node);
        }
    }

    void buildFromSudoku(const vector<vector<int>>& board)
    {
        for (int row = 0; row < SUDOKU_SIZE; row++)
        {
            for (int col = 0; col < SUDOKU_SIZE; col++)
            {
                int val = board[row][col];
                if (val != 0)
                {
                    addSudokuRow(row, col, val);
                }
                else
                {
                    for (int num = 1; num <= SUDOKU_SIZE; num++)
                    {
                        addSudokuRow(row, col, num);
                    }
                }
            }
        }
    }

    void addSudokuRow(int row, int col, int num)
    {
        int box = (row / 3) * 3 + (col / 3);
        int rowID = (row * SUDOKU_SIZE + col) * SUDOKU_SIZE + (num - 1);

        vector<int> cols = {
            row * SUDOKU_SIZE + col,                                  // Cell constraint
            81 + row * SUDOKU_SIZE + (num - 1),                       // Row constraint
            162 + col * SUDOKU_SIZE + (num - 1),                      // Column constraint
            243 + box * SUDOKU_SIZE + (num - 1)                       // Box constraint
        };

        addRow(rowID, cols);
    }

    void cover(Column* col)
    {
        col->R->L = col->L;
        col->L->R = col->R;
        for (Node* i = col->D; i != col; i = i->D)
        {
            for (Node* j = i->R; j != i; j = j->R)
            {
                j->D->U = j->U;
                j->U->D = j->D;
                j->C->size--;
            }
        }
    }

    void uncover(Column* col)
    {
        for (Node* i = col->U; i != col; i = i->U)
        {
            for (Node* j = i->L; j != i; j = j->L)
            {
                j->C->size++;
                j->D->U = j;
                j->U->D = j;
            }
        }
        col->R->L = col;
        col->L->R = col;
    }

    bool search(int k)
    {
        if (root->R == root)
        {
            return true; // Found solution
        }

        Column* col = nullptr;
        int minSize = INT_MAX;
        for (Column* c = static_cast<Column*>(root->R); c != root; c = static_cast<Column*>(c->R))
        {
            if (c->size < minSize)
            {
                minSize = c->size;
                col = c;
            }
        }

        if (!col) return false;

        cover(col);
        for (Node* row = col->D; row != col; row = row->D)
        {
            solution.push_back(row);
            for (Node* j = row->R; j != row; j = j->R)
            {
                cover(j->C);
            }

            if (search(k + 1))
            {
                return true;
            }

            for (Node* j = row->L; j != row; j = j->L)
            {
                uncover(j->C);
            }
            solution.pop_back();
        }
        uncover(col);
        return false;
    }

    void printSolution()
    {
        vector<vector<int>> solved(SUDOKU_SIZE, vector<int>(SUDOKU_SIZE));
        for (Node* row : solution)
        {
            int rowID = row->rowID;
            int r = rowID / (SUDOKU_SIZE * SUDOKU_SIZE);
            int c = (rowID / SUDOKU_SIZE) % SUDOKU_SIZE;
            int n = rowID % SUDOKU_SIZE + 1;
            solved[r][c] = n;
        }
        cout << "Solved Sudoku:\n";
        for (int i = 0; i < SUDOKU_SIZE; i++)
        {
            for (int j = 0; j < SUDOKU_SIZE; j++)
            {
                cout << solved[i][j] << " ";
            }
            cout << endl;
        }
    }
};