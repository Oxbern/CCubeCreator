#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "general.h"

#include "ccubedisplay.h"
#include "database.h"
#include "deselectabletreeview.h"
#include "descriptioneditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    friend class CCubeDisplay;
    Q_OBJECT

    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();


    private:

        Ui::MainWindow *    _ui;
        Database *          _dataBase;
        QJsonObject         _clipBoard; //Copy, cut...
        bool                _modified;
        QString             _dbFileName;
        Pattern *           _currentPattern;

        //Undo/redo
        QUndoStack *        _undoStack;
        QUndoView *         _undoView;
        void createUndoActions();

        void closeEvent(QCloseEvent *event);


    public slots:

        void contextualMenuTreeView(const QPoint& point);

        //Actions
        void addGroup();
        void addPattern();
        void addNode(DbNodeType type);
        void deleteNode();
        void renameNode();
        void moveUp();
        void moveDown();
        void move(bool up = true);
        void cut();
        void copy();
        void paste();

        //Main events
        void resetProject();
        void projectModified();
        void projectIsNotModified();
        void newProject();
        void save();
        void saveAs();
        void open();
        void quit();

        //Pattern only
        void resetPattern();
        void deactivatePatternView();
        void activatePatternView();
        void updatePatternViewer(QModelIndex const & treeIndex);
        void setDisplayedPattern(Pattern * pattern = nullptr);
        void updatePatternContent();
        void updateAddOptionButtonActivation(int currentListLine);
        void updateOptionPanel(int currentListLine);

        void addOption();
        void deleteOption();
        void updateOption();
        void newDescription(QString const & text);
        void getNewImage();


    signals:

        void displayAboutWindow();

};

#endif // MAINWINDOW_H
