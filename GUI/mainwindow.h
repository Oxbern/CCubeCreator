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


    signals:

        void displayAboutWindow();

};

#endif // MAINWINDOW_H
