#include <ctime>
#include <cstdlib>
#include <QTimer>
#include <Qt>
#include <QMessageBox>
#include "sudoky.h"
#include "ui_sudoky.h"

const int PENPOINT = 16; // font for penmarks
const int PENCILPOINT = 7; // font for pencilmarks
const QString DEFPENCIL = "        \n        \n       "; // default pencilmark text (placeholder)


// SEE SUDOKY.H FOR DOCUMENTATION

Sudoky::Sudoky(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Sudoky),
    selx(-1),
    sely(-1),
    selected(NULL),
    state(-1),
    main()
{
    ui->setupUi(this);

    board_map();
    num_map();
    game_map();

    set_state(-1);

    srand(time(NULL)); // generates a seed for random number generation

    set_shortcuts();
}


Sudoky::~Sudoky()
{
    delete ui;
}


void Sudoky::board_map() {
    boardMapper = new QSignalMapper(this);

    int but = 1;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            pencil[i][j] = DEFPENCIL;
            unlocked[i][j] = false;
            buttons[i][j] = findChild<QPushButton *>("pushButton_" + QString::number(but));
            ++but;
            buttons[i][j]->setText(NULL);

            connect(buttons[i][j], SIGNAL(clicked()), boardMapper, SLOT(map()));
            boardMapper->setMapping(buttons[i][j], i*10 + j);
        }
    }

    connect(boardMapper, SIGNAL(mapped(int)), this, SIGNAL(board_clicked(int)));
    connect(this, SIGNAL(board_clicked(int)), this, SLOT(board_click(int)));
}


void Sudoky::num_map() {
    numMapper = new QSignalMapper(this);

    for (int i = 0; i < 10; ++i) {
        QPushButton *tempButton = findChild<QPushButton *>("numButton" + QString::number(i));
        connect(tempButton, SIGNAL(clicked()), numMapper, SLOT(map()));
        numMapper->setMapping(tempButton, i);
    }

    connect(numMapper, SIGNAL(mapped(int)), this, SIGNAL(num_clicked(int)));
    connect(this, SIGNAL(num_clicked(int)), this, SLOT(num_click(int)));
}


void Sudoky::game_map() {
    gameMapper = new QSignalMapper(this);

    connect(ui->actionEasy, SIGNAL(triggered()), gameMapper, SLOT(map()));
    gameMapper->setMapping(ui->actionEasy, 0);

    connect(ui->actionMedium, SIGNAL(triggered()), gameMapper, SLOT(map()));
    gameMapper->setMapping(ui->actionMedium, 1);

    connect(ui->actionDifficult, SIGNAL(triggered()), gameMapper, SLOT(map()));
    gameMapper->setMapping(ui->actionDifficult, 2);

    connect(gameMapper, SIGNAL(mapped(int)), this, SIGNAL(game_triggered(int)));
    connect(this, SIGNAL(game_triggered(int)), this, SLOT(game_trigger(int)));
}


void Sudoky::set_shortcuts() {
    s1 = new QShortcut(Qt::Key_1, ui->numButton1, SIGNAL(clicked()));
    s2 = new QShortcut(Qt::Key_2, ui->numButton2, SIGNAL(clicked()));
    s3 = new QShortcut(Qt::Key_3, ui->numButton3, SIGNAL(clicked()));
    s4 = new QShortcut(Qt::Key_4, ui->numButton4, SIGNAL(clicked()));
    s5 = new QShortcut(Qt::Key_5, ui->numButton5, SIGNAL(clicked()));
    s6 = new QShortcut(Qt::Key_6, ui->numButton6, SIGNAL(clicked()));
    s7 = new QShortcut(Qt::Key_7, ui->numButton7, SIGNAL(clicked()));
    s8 = new QShortcut(Qt::Key_8, ui->numButton8, SIGNAL(clicked()));
    s9 = new QShortcut(Qt::Key_9, ui->numButton9, SIGNAL(clicked()));
    s0 = new QShortcut(Qt::Key_Delete, ui->numButton0, SIGNAL(clicked()));
    sP = new QShortcut(Qt::Key_Tab, this, SLOT(switch_input()));
    sU = new QShortcut(Qt::Key_Up, this, SLOT(nav_up()));
    sD = new QShortcut(Qt::Key_Down, this, SLOT(nav_down()));
    sR = new QShortcut(Qt::Key_Right, this, SLOT(nav_right()));
    sL = new QShortcut(Qt::Key_Left, this, SLOT(nav_left()));
}


void Sudoky::board_click(int ind) {
    int y = ind / 10; // ind = y*10 + x
    int x = ind % 10;

    if (selected) {
        selected->setChecked(false);
    }

    if (x == selx && y == sely) {
        selected = NULL;
        selx = -1;
        sely = -1;
        return;
    }

    selx = x;
    sely = y;
    selected = buttons[sely][selx];
    selected->setChecked(true);
}


void Sudoky::num_click(int num) {
    if (selected == NULL || !unlocked[sely][selx]) {
        return;
    }

    if (ui->penRadio->isChecked()) {
        if (selected->text() == QString::number(num) || num == 0) {
            selected->setText(NULL);
        } else {
            selected->setText(QString::number(num));
        }
        QFont f = selected->font();
        f.setPointSize(PENPOINT);
        selected->setFont(f);
    } else {
        QString *penc = &pencil[sely][selx];
        if (num == 0) {
            *penc = DEFPENCIL;
        } else if (penc->at((num - 1)*3).digitValue() == num) { // num is present in the pencilmark
            penc->replace(((num - 1)*3), 1, ' '); // replace num with ' ' in the pencilmark
        } else { // num is not in the pencilmark
            penc->replace(((num - 1)*3), 1, num + '0'); // replace the corresponding ' ' with num in the pencilmark
        }
    }

    if (selected->text().length() != 1) { // no penmark
        selected->setText(pencil[sely][selx]); // show pencilmark
        QFont f2 = selected->font();
        f2.setPointSize(PENCILPOINT);
        selected->setFont(f2);
    }
}


void Sudoky::reset_pencil() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            pencil[i][j] = DEFPENCIL;
        }
    }
}


void Sudoky::game_trigger(int diff) {
    if (selected) {
        selected->setChecked(false);
        selected = NULL;
        selx = -1;
        sely = -1;
    }

    reset_pencil();

    int blanks = 41 + (rand()%5) + (5*diff); // easy has 41-45 blank spots
                                             // med 46-50, diff 51-55

    do {
        main.clear();
    } while (generate(&main, blanks) != blanks); // generate doesn't always succeed at finding
                                                 // the given number of blank spots

    set_state(diff);
    update_label();

    main.solve();
}


void Sudoky::display_sudoku() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (main.board[j][i] != 0) {
                buttons[j][i]->setText(QString::number(main.board[j][i]));
                unlocked[j][i] = false;
                buttons[j][i]->setStyleSheet("QPushButton {color: grey;}");
            } else {
                buttons[j][i]->setText(NULL);
                unlocked[j][i] = true;
                buttons[j][i]->setStyleSheet("QPushButton {color: black;}");
            }

            QFont f = buttons[j][i]->font();
            f.setPointSize(PENPOINT);
            buttons[j][i]->setFont(f);
        }
    }
}


void Sudoky::on_actionCustom_triggered() {
    set_state(4);
    update_label();
}


void Sudoky::on_finishButton_clicked() {
    if (0 <= state && state <= 3) {
        bool equal = true;
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (buttons[i][j]->text() != QString::number(main.board[i][j])) {
                    equal = false;
                    break;
                }
            }
        }

        if (equal) { // board matches solution
            display_sudoku();
            set_state(-1);
            ui->label->setText("Sudoku solved");
        } else {
            ui->label->setText("Invalid solution");
            QTimer::singleShot(3000, this, SLOT(update_label()));
        }

    } else if (state == 4) {
        main.clear();
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (buttons[i][j]->text().length() != 0) {
                    main.insert(i, j, buttons[i][j]->text().toInt()); // add numbers on the board to main
                }
            }
        }

        int solutions = main.solve();

        if (solutions == 0) {
            ui->label->setText("Invalid: No solutions");
            QTimer::singleShot(3000, this, SLOT(update_label()));
            main.clear();
        } else if (solutions == 1) {
            set_state(3);
            update_label();
        } else {
            ui->label->setText("Invalid: Multiple solutions");
            QTimer::singleShot(5000, this, SLOT(update_label()));
            main.clear();
        }
    }
}


void Sudoky::update_label() {
    if (state == 4) {
        ui->label->setText("Create puzzle");
    } else if (state == 0) {
        ui->label->setText("Easy");
    } else if (state == 1) {
        ui->label->setText("Medium");
    } else if (state == 2) {
        ui->label->setText("Difficult");
    } else if (state == 3) {
        ui->label->setText("Custom");
    }
}


void Sudoky::switch_input() {
     if (ui->pencilRadio->isChecked()) {
         ui->penRadio->setChecked(true);
     } else {
         ui->pencilRadio->setChecked(true);
     }
}


void Sudoky::set_state(int st) {
    if (st == -1) {
        if (selected) {
            selected->setChecked(false);
        }
        selected = NULL;
        selx = -1;
        sely = -1;
        ui->pencilRadio->setCheckable(true);
        ui->penRadio->setCheckable(true);
        ui->finishButton->setEnabled(false);
        ui->finishButton->setText("Finish");
        ui->actionSolve->setEnabled(false);
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                unlocked[i][j] = false;
            }
        }
    } else if (st == 4) {
        if (selected) {
            selected->setChecked(false);
        }
        selected = NULL;
        selx = -1;
        sely = -1;
        ui->pencilRadio->setCheckable(false);
        ui->penRadio->setCheckable(true);
        ui->penRadio->setChecked(true);
        ui->finishButton->setEnabled(true);
        ui->finishButton->setText("Start game");
        ui->actionSolve->setEnabled(false);

        main.clear();

        display_sudoku();

        reset_pencil();
    } else { // starting a new game
        if (selected) {
            selected->setChecked(false);
        }
        selected = NULL;
        selx = -1;
        sely = -1;

        ui->pencilRadio->setCheckable(true);
        ui->penRadio->setCheckable(true);
        ui->finishButton->setEnabled(true);
        ui->finishButton->setText("Finish");
        ui->actionSolve->setEnabled(true);

        if (st == 3) { // game was custom-made
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if (buttons[i][j]->text().length() == 1) { // spots that are filled
                        unlocked[i][j] = false;
                        buttons[i][j]->setStyleSheet("QPushButton {color: grey;}");
                    }
                    QFont f = buttons[j][i]->font();
                    f.setPointSize(PENPOINT);
                    buttons[j][i]->setFont(f);
                }
            }
        } else {
            display_sudoku();
        }

        reset_pencil();
    }
    state = st;
}


void Sudoky::nav_up()  {
    if (selected == NULL) {
        return;
    }

    selected->setChecked(false);

    --sely;
    if (sely == -1) {
        sely = 8;
        --selx;
        if (selx == -1) {
            selx = 8;
        }
    }

    selected = buttons[sely][selx];
    selected->setChecked(true);
}


void Sudoky::nav_down() {
    if (selected == NULL) {
        return;
    }

    selected->setChecked(false);

    ++sely;
    if (sely == 9) {
        sely = 0;
        ++selx;
        if (selx == 9) {
            selx = 0;
        }
    }

    selected = buttons[sely][selx];
    selected->setChecked(true);
}


void Sudoky::nav_right() {
    if (selected == NULL) {
        return;
    }

    selected->setChecked(false);

    ++selx;
    if (selx == 9) {
        selx = 0;
        ++sely;
        if (sely == 9) {
            sely = 0;
        }
    }

    selected = buttons[sely][selx];
    selected->setChecked(true);
}


void Sudoky::nav_left() {
    if (selected == NULL) {
        return;
    }

    selected->setChecked(false);

    --selx;
    if (selx == -1) {
        selx = 8;
        --sely;
        if (sely == -1) {
            sely = 8;
        }
    }

    selected = buttons[sely][selx];
    selected->setChecked(true);
}


void Sudoky::on_actionSolve_triggered() {
    if (0 <= state && state <= 3) {
        display_sudoku();
        set_state(-1);
        ui->label->setText("Start game (Game>New)");
    }
}


void Sudoky::on_actionHow_to_Use_triggered() {
   QMessageBox::about(this, "How to play",
                       "<html><b>Start a new game</b> - Select 'Game > New':"
                       "<blockquote>- Easy/Medium/Difficult: Automatically generates a new puzzle.<br>"
                       "- Custom: Allows user to fill in a custom puzzle. Once filled in, select 'Start Game' to begin solving (puzzle must be valid).</blockquote>"
                       "<b>Solving a puzzle</b>:"
                       "<blockquote>- To input a number, select the desired position on the board.<br>"
                       "- Use 'Pen' to fill the selected position.<br>"
                       "- Use 'Pencil' to add/remove pencilmarks in the selected position.<br>"
                       "- Pencilmarks will only show if there is no penmark.<br>"
                       "- Select 'Finish' when puzzle has been completed.<br>"
                       "- Select 'Game > Solve' to end game and display solution.<br></html>");
}


void Sudoky::on_actionHow_to_Play_triggered() {
    QMessageBox::about(this, "How to solve",
                       "<html>A Sudoku board constists of 81 squares.  These squares form 9 columns, 9 rows, and 9 3x3 grids.<br><br>"
                       "To solve a Sudoku, each square must be filled with some number from 1 to 9, such that:"
                       "<blockquote>- Each column contains every integer from 1 to 9<br>"
                       "- Each row contains every integer from 1 to 9<br>"
                       "- Each grid contains every integer from 1 to 9</html>");

}
