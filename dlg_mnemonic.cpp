#include "dlg_mnemonic.h"
#include "ui_dlg_mnemonic.h"
#include "mainwindow.h"
extern "C"{
#include "bip39.h"
}
dlg_mnemonic::dlg_mnemonic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_mnemonic)
{
    ui->setupUi(this);\
    const char *tmp = mnemonic_generate(128);
    QString str(tmp);
    ui->textEdit->setText(str);
}

dlg_mnemonic::~dlg_mnemonic()
{
    delete ui;
}

void dlg_mnemonic::on_pushButton_clicked()
{
    this->close();
}

