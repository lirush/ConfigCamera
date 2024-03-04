#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Config v1.0");
    w.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    QSize s;
    s.setHeight(569); s.setWidth(800);
    w.setFixedSize(s);
    w.show();
    return a.exec();
}
