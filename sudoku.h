#ifndef SUDOKU_H
#define SUDOKU_H

// NOTE: the seed for rand() must be set before using this module.
//       use the command 'srand(time(NULL))'
//       include <cstdlib> for rand() and srand() , <ctime> for time().

class Sudoku {
public:
    // constructor for sudoku class - no input required
    // sol_count is set to -1, board is 0-filled, poss is properly filled
    Sudoku();

    // stores the value at each position of the board, 0 means blank
    int board[9][9];    

    // add_to_grid(row, col, val) adds val to this at position row, col. The
    //    possibilities of all affected items is then modified. The total number
    //    of possibilities of the element at r, c is changed to -1.
    // requires: 0 <= row, col <= 8
    //           1 <= val <= 9
    void insert(int row, int col, int val);

    // remove(row, col) changes the spot in grd at row, col
    //    to 0, and modifies the possibilities of all affected spots
    // requires: 0 <= row, col <= 8
    int remove(int row, int col);

    // clear() resets all the data to the same state as the constructor
    void clear();

    // solve() solves the puzzle if possible, and returns 0 if there are
    //    no solutions, 1 if there is only one, and 2 if there are multiple.
    int solve();

    // valid() returns true if the entries in this->board are
    //    valid (no duplicates in rows, columns or squares)
    bool valid() const;

    // copy(cpy) gives this->board the same values as
    //    cpy.board, and properly fills this->poss
    void copy(Sudoku cpy);

private:
    // each length-10 array corresponds to one board position
    // the first element of each array holds the total number of
    //    possibilities, or -1 if the board position has been filled
    // the following 9 store the integers 1 through 9 in order,
    //    some replaced by 0 if they are no longer a possibility
    int poss[9][9][10];

    // set to -1 if solve() should only find a solution.
    // set to 0 if solve() should check if multiple solutions exist
    // during the latter, sol_count stores the number of solutions found
    int sol_count;

    // find_sol() uses mutual recursion with find_sol_pos() to solve this. Returns
    //    true if solved, false if unsolvable.
    // requires: puzzle must be valid
    bool find_sol();

    // find_sol_pos(r, c) uses mutual recursion with find_sol()
    //    See find_sol() for more details.
    // requires: 0 <= row, col <= 8
    bool find_sol_pos(int r, int c);

    // find_least_poss(row, col) find the empty spot in grd with the least
    //    possibilities, and stores its position in row, col. returns false if no
    //    empty spots are found, true otherwise.
    bool find_least_poss(int *row, int *col) const;

    // fill_poss(r, c) re-evluates the possibilities of
    //    the position at row r, column c in grd
    // requires: 0 <= r, c <= 8
    void fill_poss(int r, int c);

    //sudoku_filled() returns true if all spots in this->board have been filled (with non-0's)
    bool sudoku_filled() const;
};


// generate(sud, max_blanks) returns a valid sudoku (has only 1 solution)
//    with at most max_blanks empty spots
// requires: sud->board is empty (0-filled)
//           max_blanks >= 0
int generate(Sudoku *sud, int max_blanks);

#endif // SUDOKU_H




