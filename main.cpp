#include "acceuil.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    acceuil w;
    w.show();
    return a.exec();
}
