#ifndef DLG_CREATE_ACCOUNT_H
#define DLG_CREATE_ACCOUNT_H

#include <QDialog>

namespace Ui {
class dlg_create_account;
}

class dlg_create_account : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_create_account(QWidget *parent = 0);
    ~dlg_create_account();

private slots:
    void on_create_clicked();

    void on_recover_clicked();

private:
    Ui::dlg_create_account *ui;
};

#endif // DLG_CREATE_ACCOUNT_H
