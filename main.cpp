#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setFixedSize(400, 200);
    w.setWindowFlags(Qt::WindowCloseButtonHint);


    w.show();

    return a.exec();
}
