#-------------------------------------------------
#
# Project created by QtCreator 2016-04-30T15:05:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sudoky
TEMPLATE = app

RC_ICONS = Sudoky_Icon.ico

SOURCES += main.cpp\
        sudoky.cpp \
    sudoku.cpp

HEADERS  += \
    sudoky.h \
    sudoku.h

FORMS    += sudoky.ui
