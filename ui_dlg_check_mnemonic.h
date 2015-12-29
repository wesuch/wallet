/********************************************************************************
** Form generated from reading UI file 'dlg_check_mnemonic.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_CHECK_MNEMONIC_H
#define UI_DLG_CHECK_MNEMONIC_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_dlg_check_mnemonic
{
public:
    QTextEdit *textEdit;
    QPushButton *pushButton;
    QLabel *label;

    void setupUi(QDialog *dlg_check_mnemonic)
    {
        if (dlg_check_mnemonic->objectName().isEmpty())
            dlg_check_mnemonic->setObjectName(QStringLiteral("dlg_check_mnemonic"));
        dlg_check_mnemonic->resize(400, 300);
        textEdit = new QTextEdit(dlg_check_mnemonic);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(70, 60, 231, 101));
        pushButton = new QPushButton(dlg_check_mnemonic);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(230, 180, 75, 23));
        label = new QLabel(dlg_check_mnemonic);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 30, 71, 20));

        retranslateUi(dlg_check_mnemonic);

        QMetaObject::connectSlotsByName(dlg_check_mnemonic);
    } // setupUi

    void retranslateUi(QDialog *dlg_check_mnemonic)
    {
        dlg_check_mnemonic->setWindowTitle(QApplication::translate("dlg_check_mnemonic", "Dialog", 0));
        pushButton->setText(QApplication::translate("dlg_check_mnemonic", "\345\256\214\346\210\220", 0));
        label->setText(QApplication::translate("dlg_check_mnemonic", "\350\276\223\345\205\245\345\212\251\350\256\260\350\257\215\357\274\232", 0));
    } // retranslateUi

};

namespace Ui {
    class dlg_check_mnemonic: public Ui_dlg_check_mnemonic {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_CHECK_MNEMONIC_H
