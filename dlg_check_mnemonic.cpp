#include "dlg_check_mnemonic.h"
#include "ui_dlg_check_mnemonic.h"
#include <QMessageBox>
extern "C"{
#include "bip39.h"
}

dlg_check_mnemonic::dlg_check_mnemonic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_check_mnemonic)
{
    ui->setupUi(this);
}

dlg_check_mnemonic::~dlg_check_mnemonic()
{
    delete ui;
}

void dlg_check_mnemonic::on_pushButton_clicked()
{
    if(!mnemonic_check(ui->textEdit->toPlainText().toLocal8Bit().data()))
    {
        QMessageBox::critical(this, "Error", "Text is not a valid BIP39 mnemonic.", QMessageBox::Ok);
        return;
    }
    else {
        QMessageBox::critical(this, "success", "recover account success.", QMessageBox::Ok);
        this->close();
    }

}
