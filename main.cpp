#include "bdrzhd.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BDRZHD w;
    w.show();
    return a.exec();
}
