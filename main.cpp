#include <QApplication>
#include "sudoky.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Sudoky w;
    w.show();

    return a.exec();
}
