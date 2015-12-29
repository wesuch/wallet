#ifndef DLG_MNEMONIC_H
#define DLG_MNEMONIC_H

#include <QDialog>

namespace Ui {
class dlg_mnemonic;
}

class dlg_mnemonic : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_mnemonic(QWidget *parent = 0);
    ~dlg_mnemonic();

private slots:
    void on_pushButton_clicked();

private:
    Ui::dlg_mnemonic *ui;
};

#endif // DLG_MNEMONIC_H
