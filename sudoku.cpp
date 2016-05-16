#include <ctime>
#include <cstdlib>
#include <iostream>
#include "sudoku.h"

// see sudoku.h for documentation

Sudoku::Sudoku(): sol_count(-1) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            board[i][j] = 0;
            poss[i][j][0] = 9;
            for (int k = 1; k < 10; ++k) {
                poss[i][j][k] = k;
            }
        }
    }
}


void Sudoku::clear() {
    sol_count = -1;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            board[i][j] = 0;
            poss[i][j][0] = 9;
            for (int k = 1; k < 10; ++k) {
                poss[i][j][k] = k;
            }
        }
    }
}


void Sudoku::insert(int row, int col, int val) {
    board[row][col] = val;

    for (int i = 0; i < 9; ++i) {
        if (poss[row][i][val] != 0) {
            poss[row][i][val] = 0;
            if (poss[row][i][0] > 0) {
                --poss[row][i][0];
            }
        }

        if (poss[i][col][val] != 0) {
            poss[i][col][val] = 0;
            if (poss[i][col][0] > 0) {
                --poss[i][col][0];
            }
        }

        if (poss[3*(row/3) + i%3][3*(col/3) + i/3][val] != 0) {
            poss[3*(row/3) + i%3][3*(col/3) + i/3][val] = 0;
            if (poss[3*(row/3) + i%3][3*(col/3) + i/3][0] > 0) {
                --poss[3*(row/3) + i%3][3*(col/3) + i/3][0];
            }
        }
    }

    poss[row][col][0] = -1;
}


int Sudoku::remove(int row, int col) {
    int val = board[row][col];

    board[row][col] = 0;

    for (int i = 0; i < 9; ++i) {
        fill_poss(row, i);
        fill_poss(i, col);
        fill_poss(3*(row/3) + i%3, 3*(col/3) + i/3);
    }

    return val;
}


bool Sudoku::valid() const {
    for (int i = 0; i < 9; ++i) {
        int row[9] = {0};
        int col[9] = {0};
        int sqr[9] = {0};

        int y = 3 * (i/3); // loops through the top-left position
        int x = 3 * (i%3); //    of each of the 9 3x3 grids

        for (int j = 0; j < 9; ++j) {

            int val = board[i][j];
            if (val != 0) {
                if (row[val - 1] != 0) {
                    return false;
                } else {
                    row[val - 1] = 1;
                }
            }

            val = board[j][i];
            if (val != 0) {
                if (col[val - 1] != 0) {
                   return false;
                } else {
                    col[val - 1] = 1;
                }
            }

            // loops through the 9 positions of the current
            //    3x3 square (starting at (y, x))
            val = board[y + (j/3)][x + (j%3)];
            if (val != 0) {
                if (sqr[val - 1] != 0) {
                    return false;
                } else {
                    sqr[val - 1] = 1;
                }
            }
        }
    }

    return true;
}


int Sudoku::solve() {
    if (!valid()) {
        return 0;
    }

    sol_count = 0;
    find_sol();
    int result = sol_count;

    sol_count = -1;
    if (result == 1) {
        find_sol();
    }

    return result;
}


bool Sudoku::find_sol() {
    int r = 0;
    int c = 0;

    if (!find_least_poss(&r, &c)) { // base case - no more possibilities
        if (!sudoku_filled()) { // sudoku is not full, thus this solution is invalid
            return false;
        } // sudoku is full, so a solution has been found

        if (sol_count == -1) { // only looking for solution, which we have found
            return true;
        } // counting solutions

        ++sol_count; // add to the count of solutions

        if (sol_count >= 2) {
            return true; // found multiple solutions, can return
        } else {
            return false; // only found one solution, keep looking for more
        }
    }

    return find_sol_pos(r, c); // recursive step - try filling position r, c
}


bool Sudoku::find_sol_pos(int r, int c) {
    // rd is the first number we try to add to position (r, c)
    // this is done so that filling the sudoku is random

    int rd = rand() % 9 + 1;

    for (int i = 1; i <= 9; ++i) {
        if (poss[r][c][rd] != 0) { // rd is a possibility
            insert(r, c, rd);
            if (find_sol() == true) { // valid solution
                return true;
            } // invalid solution
            remove(r, c);
        }
        ++rd; // try next value
        if (rd == 10) { // loop back to 1 if reached 10
            rd = 1;
        }
    }

    return false; // tried all possibilities, no solution
}


bool Sudoku::sudoku_filled() const {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[j][i] == 0) {
                return false;
            }
        }
    }

    return true;
}


bool Sudoku::find_least_poss(int *row, int *col) const {
    int min_poss = 10;

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (poss[r][c][0] != -1 && poss[r][c][0] <= min_poss) {

                // if number of possibilities is equal to the min, then we skip it
                //     half of the time (makes solution for random)
                if (poss[r][c][0] == min_poss && rand() % 2 == 0) {
                    continue;
                }

                min_poss = poss[r][c][0];
                *row = r;
                *col = c;
            }
        }
    }

    if (min_poss == 10) {
        return false;
    } else {
        return true;
    }
}


void Sudoku::fill_poss(int r, int c) {
    poss[r][c][0] = 9;
    for (int i = 1; i <= 9; ++i) {
        poss[r][c][i] = i;
    }

    for (int i = 0; i < 9; ++i) {
        int val_col = board[i][c];
        int val_row = board[r][i];
        // val_qdr loops through all 9 values of the 3x3 grid containing (r, c)
        int val_qdr = board[3*(r/3) + i%3][3*(c/3) + i/3];

        if (val_col != 0 && poss[r][c][val_col] != 0) {
            poss[r][c][val_col] = 0;
            --poss[r][c][0];
        }

        if (val_row != 0 && poss[r][c][val_row] != 0) {
            poss[r][c][val_row] = 0;
            --poss[r][c][0];
        }

        if (val_qdr != 0 && poss[r][c][val_qdr] != 0) {
            poss[r][c][val_qdr] = 0;
            --poss[r][c][0];
        }
    }

    if (board[r][c] != 0) {
        poss[r][c][board[r][c]] = board[r][c];
        poss[r][c][0] = -1;
    }

}


void Sudoku::copy(Sudoku cpy) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (cpy.board[j][i] != 0) {
                insert(j, i, cpy.board[j][i]);
            }
        }
    }
}


int generate(Sudoku *sud, int max_blanks) {
    sud->solve(); // arbitrarilly solve sud

    int blanks = 0;
    while (blanks < max_blanks) {
        int start_y = rand() % 9; // find a random position
        int start_x = rand() % 9;

        int y = start_y;
        int x = start_x;

        bool tried_first = false;
        do { // loops until we find an item we can remove
            if (sud->board[y][x] != 0) {
                int removed = sud->remove(y, x);

                Sudoku tester;
                tester.copy(*sud);

                if (tester.solve() == 1) {
                    break; // removing this item worked, we can exit the loop
                }
                // removing the current item made the puzzle invalid, so we add it back
                sud->insert(y, x, removed);
            }

            tried_first = true; // we tried the first position, no succes

            if (x == 8 && y == 8) { // increment x and y (loops around)
                x = 0;
                y = 0;
            } else if (x == 8) {
                x = 0;
                ++y;
            } else {
                ++x;
            }
        } while(x != start_x || y != start_y); // loop until we try all positions (or we break)

        if (tried_first && x == start_x && y == start_y) { // looped through all possibilities,
            break;                                         //     none can be removed
        } else {
            ++blanks;
        }
    } // loop until we have removed max_blanks items (or no more possibilities)

    return blanks;
}

