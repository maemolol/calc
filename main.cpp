#include <QApplication>
#include "mainwindow.h"

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow calc;
    calc.show();
    return app.exec();
}
