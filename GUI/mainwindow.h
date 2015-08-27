#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "general.h"

#include "ccubedisplay.h"
#include "database.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();


    private:

        Ui::MainWindow *    _ui;
        Database *          _dataBase;

        //Actions for the undo stack
        QAction * addNode;
        QAction * deleteNode;
        QAction * moveNode;
        QAction * renameNode;

        QAction * setLeds;
        QAction * changeImagePath;
        QAction * changeDescription;

        QAction * addOption;
        QAction * deleteOption;
        QAction * changeOption; //TODO

        QAction * undoAction;
        QAction * redoAction;


    public slots:

        void contextualMenuTreeView(const QPoint& point);


    signals:

        void displayAboutWindow();

};

#endif // MAINWINDOW_H
