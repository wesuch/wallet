#include "dlg_create_account.h"
#include "ui_dlg_create_account.h"

extern "C" {
#include "bip39.h"
}

dlg_create_account::dlg_create_account(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_create_account)
{
    ui->setupUi(this);

    const char *tmp = mnemonic_generate(128);
    QString str(tmp);
    ui->editMnemonic->setText(str);
    ui->editMnemonic->setEnabled(false);
}

dlg_create_account::~dlg_create_account()
{
    delete ui;
}
