#ifndef SUDOKY_H
#define SUDOKY_H

#include <QMainWindow>
#include <QPushButton>
#include <QSignalMapper>
#include <QShortcut>
#include "Sudoku.h"

namespace Ui {
class Sudoky;
}

class Sudoky : public QMainWindow
{
    Q_OBJECT

public:    
    explicit Sudoky(QWidget *parent = 0);

    ~Sudoky();

signals:
    // emitted when a position on the board is clicked
    void board_clicked(int);

    // emitted when a number is pressed (keyboard oron-screen num pad)
    void num_clicked(int);

    // emitted when a new game is started (easy, med or diff)
    void game_triggered(int);

private slots:
    // board_click(ind) deselects the previously selected board position
    //    and selects a new one, correspondng to ind.
    // requires: 0 <= ind / 10 <= 8
    //           0 <= ind % 10 <= 8
    void board_click(int);

    // num_click(num) fills the currently selected position with num if penRadio is
    //    selected, and toggles num in the pencilmark if pencilRadio is selected.
    //    if num == 0, the penmark or pencilmarks are removed (depending
    //    on which radio buttons is selected)
    void num_click(int);

    // game_trigger(diff) starts a new game with difficulty determined by diff
    //    (0 - easy, 1 - med, 2 - diff).  The board is reset, a puzzle is
    //    generated and displayed, and the puzzle is solved in the main member.
    //    function set_state is called to modify the interface
    void game_trigger(int);

    // on_action_Custom_triggered() sets up the board so that a custom puzzle can be input
    //   set_state is called to modify the interface
    void on_actionCustom_triggered();

    // on_finishButton_clicked() is the slot corresponding to finishButton being clicked.
    //    If a game is being played and the solution is complete, the game ends and all
    //    positions are set to 'false' in the unlocked member, otherwise a message appears
    //    saying the solution is invalid.
    //    If a custom game is being created and the current puzzle has exactly one valid
    //    solution, the custom game is started.  Otherwise, a message appears saying
    //    'no solutions' or 'multiple solutions'.  Et state is called to modify the interface.
    void on_finishButton_clicked();

    // update_label() set the text displayed of label based on the
    //    current state.  Does nothing if state is -1.
    void update_label();

    // switch_input() switched between penRadio and pencilRadio being checked
    void switch_input();

    // nav_up() deselects the current selected board position
    //    and selects the one above it (wraps around)
    void nav_up();

    // nav_down() deselects the current selected board position
    //    and selects the one balow it (wraps around)
    void nav_down();

    // nav_right() deselects the current selected board position
    //    and selects the one to the right of it (wraps around)
    void nav_right();

    // nav_left() deselects the current selected board position
    //    and selects the one to the left of it (wraps around)
    void nav_left();

    // on_actionSolve_triggered() ends the current game and displays the solution
    //    (if a game is being played).  set_state is called to modify the interface.
    void on_actionSolve_triggered();

    // on_actionHow_to_Use_triggered() displays a QMessgaeBox
    //    explaining how to use the interface.
    void on_actionHow_to_Use_triggered();

    // on_actionHow_to_Play_triggered() displays a QMessageBox
    //    explaining hot to solve a Sudoku.
    void on_actionHow_to_Play_triggered();

private:
    Ui::Sudoky *ui;

    // holds the pencilmarks for each position. Originally
    //    filled with DEFPENCIL placeholder (see sudoky.cpp)
    QString pencil[9][9];

    // each element points to one button on the board
    QPushButton *buttons[9][9];

    // holds wether or not each board position
    //    is editable (true) or not (false)
    bool unlocked[9][9];

    // correspond to the currently selected position on the board
    //    both are -1 when none selected
    int selx;
    int sely;

    // pointer to the currently selected button
    QPushButton *selected;

    // -1 = none, 0 = easy, 1 = med, 2 = diff, 3 = custom, 4 = setting up custom
    int state;

    QSignalMapper *boardMapper;
    QSignalMapper *numMapper;
    QSignalMapper *gameMapper;

    QShortcut *s1;
    QShortcut *s2;
    QShortcut *s3;
    QShortcut *s4;
    QShortcut *s5;
    QShortcut *s6;
    QShortcut *s7;
    QShortcut *s8;
    QShortcut *s9;
    QShortcut *s0;
    QShortcut *sP;
    QShortcut *sR;
    QShortcut *sL;
    QShortcut *sU;
    QShortcut *sD;

    // holds the current puzzle (see sudoku.hpp)
    Sudoku main;

    // board_map() initialized the buttons member to contain pointers to each QPushButton on the
    //    sudoku board. Each button to "locked" ('unlocked' member is filled with false), and
    //    connects each button to the board_click slot with an integer representing its location
    void board_map();

    // num_map() maps each on-screen num. pad button to the num_click slot,
    //    with the corresponding integer
    void num_map();

    // game_map() maps the 3 'starte new game' actions (easy, medium and difficult)
    //    to the slot game_trigger, with a corresponding integer
    void game_map();

    // set_shotcuts() sets shortcuts for pressing numbers (to input numbers),
    //    arrows (to navigate board), and tab (to switch between input methods).
    void set_shortcuts();

    // reset_pencil() sets every item in the pencil
    //    member back to the DEFPENCIL placeholder
    void reset_pencil();

    // display_sudoku()  prints out the contents of main.board onto the on-screen board.
    //    Empty positions are set to black text, and 'true' in the unlocked member,
    //    and the rest are set to grey text and 'false'
    void display_sudoku();

    // set_state(st) modifies the interface based on st, and sets state to st.
    void set_state(int);
};

#endif // SUDOKY_H
