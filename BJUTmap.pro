#-------------------------------------------------
#
# Project created by QtCreator 2017-10-16T20:50:33
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BJUTmap
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    location.cpp \
    animation.cpp \
    Smtp.cpp

HEADERS  += mainwindow.h \
    navigation.h \
    Smtp.h \
    location.h \
    animation.h

FORMS    += mainwindow.ui \
    location.ui

