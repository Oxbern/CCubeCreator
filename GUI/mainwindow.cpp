#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    //Treeview setup
    _dataBase = new Database();
    _ui->treeView->setModel(_dataBase);
    _ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(contextualMenuTreeView(const QPoint &)));
    _ui->treeView->setDragEnabled(true);
    _ui->treeView->setAcceptDrops(true);
    _ui->treeView->viewport()->setAcceptDrops(true);
    _ui->treeView->setDropIndicatorShown(true);
    _ui->treeView->setDragDropMode(QAbstractItemView::InternalMove);
    _ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);

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

void MainWindow::contextualMenuTreeView(const QPoint& point)
{
    (void) point;
    QMenu *menu = new QMenu;
    menu->addAction("lol");
    menu->exec(QCursor::pos());
    /*
    QModelIndex index = view->currentIndex();

    if(view->rootIndex() == index)
    {
       // construct the context menu required for ChildItem items
    }
    else
    {
       if(view->model()->hasChildren(index)
       {
           // construct the context menu required for the RootNode item
       }
    }*/
}
