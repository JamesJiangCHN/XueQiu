#-------------------------------------------------
#
# Project created by QtCreator 2015-08-11T22:07:56
#
#-------------------------------------------------

QT       += core gui xml
QT += webkitwidgets
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XueQiu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    stock.cpp \
    stockzh.cpp

HEADERS  += mainwindow.h \
    stock.h \
    stockzh.h

FORMS    += mainwindow.ui
