#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T16:16:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qrun
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

target.path = /usr/bin/
INSTALLS  += target
fontpath.path = ~/.fonts/
fontpath.files = DroidSans.ttf
INSTALLS += fontpath
