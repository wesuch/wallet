#ifndef DLG_CHECK_MNEMONIC_H
#define DLG_CHECK_MNEMONIC_H

#include <QDialog>

namespace Ui {
class dlg_check_mnemonic;
}

class dlg_check_mnemonic : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_check_mnemonic(QWidget *parent = 0);
    ~dlg_check_mnemonic();

private slots:
    void on_pushButton_clicked();

private:
    Ui::dlg_check_mnemonic *ui;

};

#endif // DLG_CHECK_MNEMONIC_H
