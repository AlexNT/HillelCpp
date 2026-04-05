#include "calculatorwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CalculatorWindow w;
    w.resize(320, 460);
    w.show();
    return a.exec();
}
