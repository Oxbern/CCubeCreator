#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Manual layout stuff
    { //Set the size of the tree
        QList<int> sizes { 200, 500 };
        ui->splitter_4->setSizes(sizes);
    }

    { //Set the size of the opengl widget
        QList<int> sizes { 500, 150 };
        ui->splitter_2->setSizes(sizes);
    }

    { //Set the size of the option panel
        QList<int> sizes { 400, 180 };
        ui->splitter_3->setSizes(sizes);
    }

    //Connections
    connect(ui->actionAbout, SIGNAL(triggered()), this, SIGNAL(displayAboutWindow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
