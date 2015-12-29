/********************************************************************************
** Form generated from reading UI file 'dlg_create_account.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_CREATE_ACCOUNT_H
#define UI_DLG_CREATE_ACCOUNT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_dlg_create_account
{
public:
    QPushButton *recover;
    QPushButton *create;

    void setupUi(QDialog *dlg_create_account)
    {
        if (dlg_create_account->objectName().isEmpty())
            dlg_create_account->setObjectName(QStringLiteral("dlg_create_account"));
        dlg_create_account->resize(310, 194);
        recover = new QPushButton(dlg_create_account);
        recover->setObjectName(QStringLiteral("recover"));
        recover->setGeometry(QRect(190, 80, 75, 23));
        create = new QPushButton(dlg_create_account);
        create->setObjectName(QStringLiteral("create"));
        create->setGeometry(QRect(50, 80, 75, 23));

        retranslateUi(dlg_create_account);

        QMetaObject::connectSlotsByName(dlg_create_account);
    } // setupUi

    void retranslateUi(QDialog *dlg_create_account)
    {
        dlg_create_account->setWindowTitle(QApplication::translate("dlg_create_account", "Dialog", 0));
        recover->setText(QApplication::translate("dlg_create_account", "recover", 0));
        create->setText(QApplication::translate("dlg_create_account", "create", 0));
    } // retranslateUi

};

namespace Ui {
    class dlg_create_account: public Ui_dlg_create_account {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_CREATE_ACCOUNT_H
