#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    //Manual setup
    _dataBase = new Database();
    _ui->treeView->setModel(_dataBase);

    //Manual layout stuff
    { //Set the size of the tree
        QList<int> sizes { 200, 500 };
        _ui->splitter_4->setSizes(sizes);
    }

    { //Set the size of the opengl widget
        QList<int> sizes { 500, 150 };
        _ui->splitter_2->setSizes(sizes);
    }

    { //Set the size of the option panel
        QList<int> sizes { 400, 180 };
        _ui->mainSplitterRight->setSizes(sizes);
    }

    //Connections
    connect(_ui->actionAbout, SIGNAL(triggered()), this, SIGNAL(displayAboutWindow()));
    connect(_ui->btnResetView, SIGNAL(released()), _ui->ccubeDisplay, SLOT(resetView()));
    connect(_ui->btnAxes, SIGNAL(released()), _ui->ccubeDisplay, SLOT(toggleAxesDisplay()));
    connect(_ui->btnPositions, SIGNAL(released()), _ui->ccubeDisplay, SLOT(togglePositionsDisplay()));
}

MainWindow::~MainWindow()
{
    delete _ui;
}
