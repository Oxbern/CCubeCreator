#ifndef GENERAL_H
#define GENERAL_H

//Includes
#include <QDialog>
#include <QGLWidget>
#include <QMainWindow>
#include <QApplication>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>
#include <QVector3D>
#include <QtMath>
#include <QAbstractItemModel>
#include <QMimeData>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

//LEDs
#define N 9

//Colors
#define COLOR_WHITE  236.f/255.f,   240.f/255.f,    241.f/255.f,    255.f/255.f
#define COLOR_GRAY   189.f/255.f,   195.f/255.f,    199.f/255.f,    255.f/255.f
#define COLOR_MID    95.f/255.f,    102.f/255.f,    103.f/255.f,    255.f/255.f
#define COLOR_DARK   65.f/255.f,    73.f/255.f,     70.f/255.f,     255.f/255.f
#define COLOR_BLACK  9.f/255.f,     5.f/255.f,      4.f/255.f,      255.f/255.f

#define COLOR_BLUE   52.f/255.f,    152.f/255.f,    219.f/255.f,    255.f/255.f
#define COLOR_GREEN  46.f/255.f,    204.f/255.f,    113.f/255.f,    255.f/255.f
#define COLOR_ORANGE 230.f/255.f,   126.f/255.f,    34.f/255.f,     255.f/255.f
#define COLOR_YELLOW 241.f/255.f,   196.f/255.f,    15.f/255.f,     255.f/255.f
#define COLOR_RED    231.f/255.f,   76.f/255.f,     60.f/255.f,     255.f/255.f
#define COLOR_VIOLET 155.f/255.f,   89.f/255.f,     182.f/255.f,    255.f/255.f

//Messages
extern "C"
{
#define DEBUG_MSG(...)     \
    { \
        do      \
        {	   \
            qDebug() \
            << "[DEBUG] "		\
            << " (" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ") "	\
            << __VA_ARGS__;		\
        } while( 0 );     \
    }

    #define ERROR_MSG(...)     \
    { 	   \
        do      \
        {	   \
            qDebug() \
            << "[ERROR] "		\
            << " (" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ") "	\
            << __VA_ARGS__;		\
        } while( 0 );     \
    }

    #define ERROR_QUIT(...)     \
    { 	   \
        do      \
        {	   \
            ERROR_MSG(__VA_ARGS__);		\
            std::exit(EXIT_FAILURE);		\
        } while( 0 ); \
    }
} //extern C

#endif // GENERAL_H
