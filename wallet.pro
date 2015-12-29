#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T11:17:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wallet
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    dlg_create_account.cpp \


HEADERS  += mainwindow.h \
    dlg_create_account.h \

FORMS    += mainwindow.ui \
    dlg_create_account.ui


LIBS += -L$$PWD/trezor-crypto/ -ltrezor-crypto

INCLUDEPATH += $$PWD/trezor-crypto
DEPENDPATH += $$PWD/trezro-crypto

