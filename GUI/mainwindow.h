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

        //Undo/redo
        QUndoStack *        _undoStack;
        void createUndoActions();

        //Actions for the undo stack
        QAction * _actionAddGroup;
        QAction * _actionAddPattern;
        QAction * _actionDeleteNode;
        QAction * _actionMoveNode;
        QAction * _actionRenameNode;

        QAction * _actionSetLeds;
        QAction * _actionChangeImagePath;
        QAction * _actionChangeDescription;

        QAction * _actionAddOption;
        QAction * _actionDeleteOption;
        QAction * _actionChangeOption; //TODO

        QAction * _actionUndo;
        QAction * _actionRedo;

        //Actions
        void addGroup();
        void addPattern();
        void deleteNode();
        void moveNode();
        void renameNode();


    public slots:

        void contextualMenuTreeView(const QPoint& point);


    signals:

        void displayAboutWindow();

};

#endif // MAINWINDOW_H
