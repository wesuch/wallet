#include "dlg_create_account.h"
#include "ui_dlg_create_account.h"
#include "dlg_mnemonic.h"
#include "mainwindow.h"
#include "dlg_check_mnemonic.h"
extern "C" {
#include "bip39.h"
}

dlg_create_account::dlg_create_account(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_create_account)
{
    ui->setupUi(this);



}

dlg_create_account::~dlg_create_account()
{
    delete ui;
}

void dlg_create_account::on_create_clicked()
{
    this->close();
    dlg_mnemonic *mnemonic = new dlg_mnemonic(this);
    mnemonic->setWindowModality(Qt::ApplicationModal);
    mnemonic->show();
}

void dlg_create_account::on_recover_clicked()
{
    dlg_check_mnemonic *check = new dlg_check_mnemonic(this);
    check->show();
}
