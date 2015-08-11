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
        GUI/mainwindow.cpp \
    GUI/aboutwindow.cpp \
    GUI/ccubedisplay.cpp \
    Database/pattern.cpp \
    Database/group.cpp \
    Database/database.cpp \
    Database/option.cpp \
    Database/dbnode.cpp

HEADERS  += GUI/mainwindow.h \
    GUI/aboutwindow.h \
    GUI/ccubedisplay.h \
    general.h \
    Database/pattern.h \
    Database/group.h \
    Database/database.h \
    Database/option.h \
    Database/dbnode.h

FORMS    += GUI/mainwindow.ui \
    GUI/aboutwindow.ui

RESOURCES += \
    resources.qrc

INCLUDEPATH += Databse \
    GUI

win32:LIBS += -lopengl32
