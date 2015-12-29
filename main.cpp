#include "mainwindow.h"
#include "dlg_create_account.h"
#include <QApplication>
#include <dlg_create_account.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    dlg_create_account account ;
   // w.setFixedSize(400, 200);
   //w.setWindowFlags(Qt::WindowCloseButtonHint);

    w.show();
    account.setWindowModality(Qt::ApplicationModal);
    account.show();
    return a.exec();
}
