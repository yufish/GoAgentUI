#-------------------------------------------------
#
# Project created by QtCreator 2013-05-20T23:39:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = goagentui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RC_FILE += gaui.rc

include(./qtsingleapplication/src/qtsingleapplication.pri)

OTHER_FILES += \
    win.jpg
