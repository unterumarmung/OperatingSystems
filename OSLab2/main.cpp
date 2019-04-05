#include "numismatisthandbook.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NumismatistHandbook w;
    w.show();

    return a.exec();
}
