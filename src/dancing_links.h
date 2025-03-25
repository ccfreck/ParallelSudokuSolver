// Thick headerfile! Had to define the struct in here to remove an error

#ifndef DANCING_LINKS_H
#define DANCING_LINKS_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include <cstring>
#include <omp.h>
#include <chrono>
using namespace std;

const int SUDOKU_SIZE = 9;
const int NUM_CONSTRAINTS = 4;
const int NUM_ROWS = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE; // 729
const int NUM_COLS = SUDOKU_SIZE * SUDOKU_SIZE * NUM_CONSTRAINTS; // 324
const int PUZZLE_SIZE = 81;

struct Column;

struct Node
{
    Node *L, *R, *U, *D;
    Column* C;
    int rowID;

    Node() : L(this), R(this), U(this), D(this), C(nullptr), rowID(-1) {}
};

struct Column : public Node
{
    int size;
    string name;

    Column(const string& n) : size(0), name(n) { C = this; }
};

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

            col->L = prev;
            col->R = root;
            prev->R = col;
            root->L = col;
            prev = col;
        }
    }

    void addRow(int rowID, const vector<int>& colIndices)
    {
        Node* first = nullptr;
        for (int col : colIndices)
        {
            Column* column = columns[col];
            Node* node = new Node();
            node->C = column;
            node->rowID = rowID;

            node->U = column->U;
            node->D = column;
            column->U->D = node;
            column->U = node;
            column->size++;

            if (!first)
                first = node;

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
                    addSudokuRow(row, col, val);
                else
                    for (int num = 1; num <= SUDOKU_SIZE; num++)
                        addSudokuRow(row, col, num);
            }
        }
    }

    void addSudokuRow(int row, int col, int num)
    {
        int box = (row / 3) * 3 + (col / 3);
        int rowID = (row * SUDOKU_SIZE + col) * SUDOKU_SIZE + (num - 1);

        vector<int> cols = {
            row * SUDOKU_SIZE + col,
            81 + row * SUDOKU_SIZE + (num - 1),
            162 + col * SUDOKU_SIZE + (num - 1),
            243 + box * SUDOKU_SIZE + (num - 1)
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
        if (root->R == root) return true;

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
                cover(j->C);

            if (search(k + 1)) return true;

            for (Node* j = row->L; j != row; j = j->L)
                uncover(j->C);
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

        for (int i = 0; i < SUDOKU_SIZE; i++)
        {
            for (int j = 0; j < SUDOKU_SIZE; j++)
            {
                cout << solved[i][j] << " ";
                if ((j + 1) % 3 == 0 && j != SUDOKU_SIZE - 1)
                    cout << "| ";
            }
            cout << "\n";
            if ((i + 1) % 3 == 0 && i != SUDOKU_SIZE - 1)
                cout << "------+-------+------\n";
        }
    }

};

struct DLXMPI
{
    Column* root;
    vector<Column*> columns;
    vector<Node*> nodes;
    vector<Node*> solution;

    DLXMPI(int numCols)
    {
        root = new Column("root");
        columns.resize(numCols);

        Column* prev = root;
        for (int i = 0; i < numCols; i++)
        {
            Column* col = new Column("C" + to_string(i));
            columns[i] = col;

            col->L = prev;
            col->R = root;
            prev->R = col;
            root->L = col;
            prev = col;
        }
    }

    void addRow(int rowID, const vector<int>& colIndices)
    {
        Node* first = nullptr;
        for (int col : colIndices)
        {
            Column* column = columns[col];
            Node* node = new Node();
            node->C = column;
            node->rowID = rowID;

            node->U = column->U;
            node->D = column;
            column->U->D = node;
            column->U = node;
            column->size++;

            if (!first)
                first = node;

            node->L = first->L;
            node->R = first;
            first->L->R = node;
            first->L = node;

            nodes.push_back(node);
        }
    }

    void buildFromFlatPuzzle(const int* puzzle)
    {
        for (int i = 0; i < PUZZLE_SIZE; i++)
        {
            int row = i / 9;
            int col = i % 9;
            int val = puzzle[i];
            if (val != 0)
                addSudokuRow(row, col, val);
            else
                for (int num = 1; num <= 9; num++)
                    addSudokuRow(row, col, num);
        }
    }

    void addSudokuRow(int row, int col, int num)
    {
        int box = (row / 3) * 3 + (col / 3);
        int rowID = (row * SUDOKU_SIZE + col) * SUDOKU_SIZE + (num - 1);

        vector<int> cols = {
            row * SUDOKU_SIZE + col,
            81 + row * SUDOKU_SIZE + (num - 1),
            162 + col * SUDOKU_SIZE + (num - 1),
            243 + box * SUDOKU_SIZE + (num - 1)
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
        if (root->R == root) return true;

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
                cover(j->C);

            if (search(k + 1)) return true;

            for (Node* j = row->L; j != row; j = j->L)
                uncover(j->C);
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

void runDLXMPIFromMatrix(const vector<vector<int>>& matrix);

#endif // DANCING_LINKS_H
