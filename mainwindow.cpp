#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlg_create_account.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_create_Button_clicked()
{
    dlg_create_account *account = new dlg_create_account(this);
    account->show();
}
