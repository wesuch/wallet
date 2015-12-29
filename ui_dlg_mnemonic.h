/********************************************************************************
** Form generated from reading UI file 'dlg_mnemonic.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_MNEMONIC_H
#define UI_DLG_MNEMONIC_H

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

class Ui_dlg_mnemonic
{
public:
    QTextEdit *textEdit;
    QLabel *label;
    QPushButton *pushButton;

    void setupUi(QDialog *dlg_mnemonic)
    {
        if (dlg_mnemonic->objectName().isEmpty())
            dlg_mnemonic->setObjectName(QStringLiteral("dlg_mnemonic"));
        dlg_mnemonic->resize(400, 300);
        textEdit = new QTextEdit(dlg_mnemonic);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setEnabled(false);
        textEdit->setGeometry(QRect(50, 60, 281, 111));
        label = new QLabel(dlg_mnemonic);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(50, 30, 54, 12));
        pushButton = new QPushButton(dlg_mnemonic);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(250, 230, 75, 23));

        retranslateUi(dlg_mnemonic);

        QMetaObject::connectSlotsByName(dlg_mnemonic);
    } // setupUi

    void retranslateUi(QDialog *dlg_mnemonic)
    {
        dlg_mnemonic->setWindowTitle(QApplication::translate("dlg_mnemonic", "Dialog", 0));
        label->setText(QApplication::translate("dlg_mnemonic", "\345\212\251\350\256\260\350\257\215\357\274\232", 0));
        pushButton->setText(QApplication::translate("dlg_mnemonic", "\345\256\214\346\210\220", 0));
    } // retranslateUi

};

namespace Ui {
    class dlg_mnemonic: public Ui_dlg_mnemonic {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_MNEMONIC_H
