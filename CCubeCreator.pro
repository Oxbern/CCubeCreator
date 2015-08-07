#-------------------------------------------------
#
# Project created by QtCreator 2015-08-04T11:52:36
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CCubeCreator
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    aboutwindow.cpp \
    ccubedisplay.cpp

HEADERS  += mainwindow.h \
    aboutwindow.h \
    ccubedisplay.h \
    general.h

FORMS    += mainwindow.ui \
    aboutwindow.ui

RESOURCES += \
    resources.qrc
