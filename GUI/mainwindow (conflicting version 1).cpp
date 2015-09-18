#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _clipBoard = QJsonObject();
    _modified = false;

    //Undo/redo
    _undoStack = new QUndoStack(this);
    connect(_ui->actionUndo, SIGNAL(triggered()), _undoStack, SLOT(undo()));
    connect(_ui->actionRedo, SIGNAL(triggered()), _undoStack, SLOT(redo()));

    _undoView = new QUndoView(_undoStack);
    _undoView->setWindowTitle(tr("Command List"));
    //_undoView->show();
    _undoView->setAttribute(Qt::WA_QuitOnClose, false);

    //Treeview setup
    _dataBase = new Database(_undoStack);
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

    //Signals about modifications
    connect(_dataBase, SIGNAL(modified()), this, SLOT(projectModified()));
    connect(_dataBase, SIGNAL(select(QModelIndex)), _ui->treeView, SLOT(setCurrentIndex(QModelIndex)));

    /* Toolbuttons for tree view */
    //Actions
    _ui->toolBtnAddGroup->setDefaultAction(_ui->actionTreeAddGroup);
    _ui->toolBtnAddPattern->setDefaultAction(_ui->actionTreeAddPattern);
    _ui->toolBtnImport->setDefaultAction(_ui->actionTreeImport);
    _ui->toolBtnExport->setDefaultAction(_ui->actionTreeExport);

    _ui->toolBtnCut->setDefaultAction(_ui->actionTreeCut);
    _ui->toolBtnCopy->setDefaultAction(_ui->actionTreeCopy);
    _ui->toolBtnPaste->setDefaultAction(_ui->actionTreePaste);
    _ui->toolBtnDelete->setDefaultAction(_ui->actionTreeDelete);

    _ui->toolBtnRename->setDefaultAction(_ui->actionTreeRename);
    _ui->toolBtnUp->setDefaultAction(_ui->actionTreeUp);
    _ui->toolBtnDown->setDefaultAction(_ui->actionTreeDown);

    _ui->toolBtnAsRoot->setDefaultAction(_ui->actionTreeAsRoot);
    _ui->toolBtnClear->setDefaultAction(_ui->actionTreeClearPattern);

    //Connections
    connect(_ui->actionTreeAddGroup, SIGNAL(triggered()), this, SLOT(addGroup()));
    connect(_ui->actionTreeAddPattern, SIGNAL(triggered()), this, SLOT(addPattern()));
    connect(_ui->actionTreeRename, SIGNAL(triggered()), this, SLOT(renameNode()));
    connect(_ui->actionTreeDelete, SIGNAL(triggered()), this, SLOT(deleteNode()));
    connect(_ui->actionTreeUp, SIGNAL(triggered()), this, SLOT(moveUp()));
    connect(_ui->actionTreeDown, SIGNAL(triggered()), this, SLOT(moveDown()));
    connect(_ui->actionTreeCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(_ui->actionTreeCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(_ui->actionTreePaste, SIGNAL(triggered()), this, SLOT(paste()));

    /* Menu */
    //Connections
    connect(_ui->actionAbout, SIGNAL(triggered()), this, SIGNAL(displayAboutWindow()));
    connect(_ui->actionNew, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(_ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(_ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(_ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(_ui->actionExit, SIGNAL(triggered()), this, SLOT(quit()));

    /* Pattern stuff */
    //Buttons
    connect(_ui->btnResetView, SIGNAL(released()), _ui->ccubeDisplay, SLOT(resetView()));
    connect(_ui->btnAxes, SIGNAL(released()), _ui->ccubeDisplay, SLOT(toggleAxesDisplay()));
    connect(_ui->btnPositions, SIGNAL(released()), _ui->ccubeDisplay, SLOT(togglePositionsDisplay()));
    connect(_ui->btnPatternAddOption, SIGNAL(released()), this, SLOT(addOption()));
    connect(_ui->patternSelectOption, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAddOptionButtonActivation(int)));
    connect(_ui->patternOptionList, SIGNAL(currentRowChanged(int)), this, SLOT(updateOptionPanel(int)));
    //Options
    connect(_ui->optionBlinkms, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionBlinkX, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionBlinkY, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionBlinkZ, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplX1, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplX2, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplX3, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplY1, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplY2, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplY3, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplZ1, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplZ2, SIGNAL(editingFinished()), this, SLOT(updateOption()));
    connect(_ui->optionDuplZ3, SIGNAL(editingFinished()), this, SLOT(updateOption()));

    //View
    connect(_ui->treeView, SIGNAL(activated(QModelIndex)), this, SLOT(updatePatternViewer(QModelIndex)));
    connect(_ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(updatePatternViewer(QModelIndex)));
    connect(_ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(updatePatternViewer(QModelIndex)));

    //Start
    resetProject();
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _undoStack;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->quit();
}

void MainWindow::contextualMenuTreeView(const QPoint& point)
{
    QModelIndex pointedIndex = _ui->treeView->indexAt(point);

    QMenu *menu = new QMenu;

    //On group
    if (_dataBase->getItem(pointedIndex)->getType() == DbNodeType::Group)
    {
        //If not root
        if (pointedIndex != QModelIndex())
        {
            menu->insertAction(0, _ui->actionTreeCut);
            menu->insertAction(0, _ui->actionTreeCopy);
            menu->insertAction(0, _ui->actionTreePaste);
            menu->insertAction(0, _ui->actionTreeDelete);
            menu->insertAction(0, _ui->actionTreeExport);
            menu->addSeparator();
            menu->insertAction(0, _ui->actionTreeRename);
            menu->insertAction(0, _ui->actionTreeUp);
            menu->insertAction(0, _ui->actionTreeDown);
            menu->addSeparator();
            menu->insertAction(0, _ui->actionTreeAsRoot);
        }
        else //If root
        {
            menu->insertAction(0, _ui->actionTreeAddGroup);
            menu->insertAction(0, _ui->actionTreeAddPattern);
            menu->insertAction(0, _ui->actionTreePaste);
            menu->insertAction(0, _ui->actionTreeImport);
        }
    }
    //On pattern
    else if (_dataBase->getItem(pointedIndex)->getType() == DbNodeType::Pattern)
    {
        menu->insertAction(0, _ui->actionTreeCut);
        menu->insertAction(0, _ui->actionTreeCopy);
        menu->insertAction(0, _ui->actionTreePaste);
        menu->insertAction(0, _ui->actionTreeDelete);
        menu->insertAction(0, _ui->actionTreeExport);
        menu->addSeparator();
        menu->insertAction(0, _ui->actionTreeRename);
        menu->insertAction(0, _ui->actionTreeUp);
        menu->insertAction(0, _ui->actionTreeDown);
        menu->addSeparator();
        menu->insertAction(0, _ui->actionTreeClearPattern);
    }
    else
        ERROR_MSG("Uknown object type: " << (int)_dataBase->getItem(pointedIndex)->getType());

    //TODO: paste

    menu->exec(QCursor::pos());
}

/***********************************/
/* Slots for database manipulation */
/***********************************/

void MainWindow::addGroup()
{
    addNode(DbNodeType::Group);
}

void MainWindow::addPattern()
{
    addNode(DbNodeType::Pattern);
}

void MainWindow::addNode(DbNodeType type)
{
    QModelIndex parent;
    if (_dataBase->getItem(_ui->treeView->currentIndex())->getType() == DbNodeType::Group)
    {
        parent = _ui->treeView->currentIndex();
    }
    else if (_dataBase->getItem(_ui->treeView->currentIndex())->getType() == DbNodeType::Pattern)
    {
        parent = _dataBase->parent(_ui->treeView->currentIndex());
    }
    else
    {
        ERROR_MSG("Cannot add node to " << _dataBase->getItem(_ui->treeView->currentIndex())->getName() << ": has unknown type");
        return;
    }

    if (type == DbNodeType::Unknown)
    {
        ERROR_MSG("Cannot add unknown type node to " << _dataBase->getItem(_ui->treeView->currentIndex())->getName());
        return;
    }

    _undoStack->push(new Commands::InsertNode(parent, QVariant((int)type), _dataBase));
}

void MainWindow::deleteNode()
{
    if (!_ui->treeView->currentIndex().isValid())
        return; //can't remove root

    _undoStack->push(new Commands::RemoveNode(_ui->treeView->currentIndex(), QVariant(_dataBase->getItem(_ui->treeView->currentIndex())->toJson()), _dataBase));
}

void MainWindow::renameNode()
{
    if (_ui->treeView->currentIndex().isValid())
        _ui->treeView->edit(_ui->treeView->currentIndex());
}

void MainWindow::moveUp()
{
    move(true);
}

void MainWindow::moveDown()
{
    move(false);
}

void MainWindow::move(bool up)
{
    if (!_ui->treeView->currentIndex().isValid())
        return; //can't move root

    int row = _ui->treeView->currentIndex().row();
    if (up)
    {
        row--;
        if (row < 0)
            row = _dataBase->rowCount(_ui->treeView->currentIndex().parent());
    }
    else
    {
        row += 2;
        if (row > _dataBase->rowCount(_ui->treeView->currentIndex().parent()))
            row = 0;
    }

    _undoStack->push(new Commands::MoveNode(_ui->treeView->currentIndex(), row, _ui->treeView->currentIndex().parent(), _dataBase->getItem(_ui->treeView->currentIndex())->toJson(), _dataBase));
}

void MainWindow::cut()
{
    copy();
    deleteNode();
}

void MainWindow::copy()
{
    if (!_ui->treeView->currentIndex().isValid())
        return; //can't copy root

    _clipBoard = _dataBase->getItem(_ui->treeView->currentIndex())->toJson();
}

void MainWindow::paste()
{
    DEBUG_MSG("Pasting in current index " << _dataBase->getItem(_ui->treeView->currentIndex())->getName());

    _undoStack->beginMacro(QString("Paste ")+_clipBoard["name"].toString()+QString(" inside ")+_dataBase->getItem(_ui->treeView->currentIndex())->getName());

    _undoStack->push(new Commands::InsertNode(_ui->treeView->currentIndex(), QVariant((int)DbNode::getObjectType(_clipBoard)), _dataBase));
    _undoStack->push(new Commands::NodeSetContentFromJson(_dataBase->index(_dataBase->rowCount(_ui->treeView->currentIndex())-1, 0, _ui->treeView->currentIndex()), _clipBoard, _dataBase));

    _undoStack->endMacro();
}

/**************/
/* Main stuff */
/**************/

void MainWindow::resetProject()
{
    _dataBase->reset();
    _modified = false;
    deactivatePatternView();
    _dbFileName.clear();
    setWindowTitle("New database -- CCubeCreator");
}

void MainWindow::projectModified()
{
    setWindowTitle(_dataBase->getRootName()+QString(" *")+QString("  -- CCubeCreator"));
    _modified = true;
}

void MainWindow::projectIsNotModified()
{
    setWindowTitle(_dataBase->getRootName()+QString("  -- CCubeCreator"));
    _dataBase->setUnmodified();
    _modified = false;
}

void MainWindow::newProject()
{
    if (_modified)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The database has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int choice = msgBox.exec();

        switch (choice)
        {
            case QMessageBox::Save:
                this->save();
                this->resetProject();
                break;
            case QMessageBox::Discard:
                this->resetProject();
                break;
            case QMessageBox::Cancel:
                return;
                break;
            default:
                ERROR_MSG("Uknown choice " << choice << " for message box 'The database has been modified'");
                break;
        }
    }
    else
    {
        this->resetProject();
    }
}

void MainWindow::save()
{
    if (_dbFileName.isNull())
        saveAs();
    else
    {
        if (!_dataBase->save(_dbFileName))
        {
            ERROR_MSG("Could not save database to file " << _dbFileName);
        }
        else
            this->projectIsNotModified();
    }
}

void MainWindow::saveAs()
{
    QString newFileName = QFileDialog::getSaveFileName(this, QString("Save as..."), QString(), tr("CCube database files (*.ccdb)"));
    if (!_dataBase->save(newFileName))
    {
        ERROR_MSG("Could not save database to file " << _dbFileName);
    }
    else
    {
        _dbFileName = newFileName;

        const QStringList splitSlash = newFileName.split("/", QString::SkipEmptyParts);
        QString name = splitSlash.last();
        name.chop(QString(".ccdb").length());
        _dataBase->setRootName(name);

        this->projectIsNotModified();
    }
}

void MainWindow::open()
{
    QString newFileName = QFileDialog::getOpenFileName(this, QString("Open..."), QString(), tr("CCube database files (*.ccdb)"));
    if (!_dataBase->load(newFileName))
    {
        ERROR_MSG("Could not load file " << _dbFileName);
    }
    else
    {
        _dbFileName = newFileName;
        this->projectIsNotModified();
    }
}

void MainWindow::quit()
{
    if (_modified)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The database has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int choice = msgBox.exec();

        switch (choice)
        {
            case QMessageBox::Save:
                this->save();
                QApplication::quit();
                break;
            case QMessageBox::Discard:
                QApplication::quit();
                break;
            case QMessageBox::Cancel:
                return;
                break;
            default:
                ERROR_MSG("Uknown choice " << choice << " for message box 'The database has been modified'");
                break;
        }
    }
    else
    {
        QApplication::quit();
    }
}

/*****************/
/* Pattern stuff */
/*****************/

void MainWindow::resetPattern()
{
    _currentPattern->clear();
}

void MainWindow::deactivatePatternView()
{
    _ui->btnAxes->setEnabled(false);
    _ui->btnPositions->setEnabled(false);
    _ui->btnResetView->setEnabled(false);

    _ui->ccubeDisplay->setEnabled(false);

    _ui->patternImageLabel->setEnabled(false);
    _ui->btnPatternSelectImage->setEnabled(false);
    _ui->patternImagePreview->setEnabled(false);
    _ui->patternDescriptionLabel->setEnabled(false);
    _ui->patternDescription->setEnabled(false);

    _ui->patternSelectOption->setEnabled(false);
    _ui->btnPatternAddOption->setEnabled(false);
    _ui->patternOptionList->setEnabled(false);
    _ui->patternOptionPanel->setEnabled(false);
}

void MainWindow::activatePatternView()
{
    _ui->btnAxes->setEnabled(true);
    _ui->btnPositions->setEnabled(true);
    _ui->btnResetView->setEnabled(true);

    _ui->ccubeDisplay->setEnabled(true);
    _ui->ccubeDisplay->resetView();

    _ui->patternImageLabel->setEnabled(true);
    _ui->btnPatternSelectImage->setEnabled(true);
    _ui->patternImagePreview->setEnabled(true);
    _ui->patternDescriptionLabel->setEnabled(true);
    _ui->patternDescription->setEnabled(true);

    _ui->patternSelectOption->setEnabled(true);
    //_ui->btnPatternAddOption->setEnabled(true);
    _ui->patternOptionList->setEnabled(true);
    _ui->patternOptionPanel->setEnabled(true);
}

void MainWindow::updatePatternViewer(QModelIndex const & treeIndex)
{
    if (_dataBase->getItem(treeIndex)->getType() == DbNodeType::Pattern)
    {
        setDisplayedPattern(dynamic_cast<Pattern*>(_dataBase->getItem(treeIndex)));
    }
    else
        setDisplayedPattern();
}

void MainWindow::setDisplayedPattern(Pattern * pattern)
{
    _currentPattern = pattern;
    DEBUG_MSG("Current displayed pattern is " << pattern);
    updatePatternContent();
}

void MainWindow::updatePatternContent()
{
    if (_currentPattern == nullptr)
    {
        deactivatePatternView();
        return;
    }

    activatePatternView();

    //Description
    _ui->patternDescription->setPlainText(_currentPattern->getDescription());

    //Image
    _ui->patternImagePreview->setPixmap(QPixmap::fromImage(_currentPattern->getImage()));

    //Points
    _ui->ccubeDisplay->displayPoints(_currentPattern->getAllPoints());

    //Options
    int optionListIndex = _ui->patternOptionList->currentRow();
    _ui->patternOptionList->clear();
    if (_currentPattern->getNumberOfOptions() > 0)
    {
        DEBUG_MSG("Option 0 of current pattern is " << _currentPattern->getOption(0));
        DEBUG_MSG("Its text is " << _currentPattern->getOption(0)->toText());
    }
    for (int i=0 ; i<_currentPattern->getNumberOfOptions() ; i++)
    {
        DEBUG_MSG("Getting option from pattern " << _currentPattern);
        DEBUG_MSG("New option text " << _currentPattern->getOption(i)->toText());
        _ui->patternOptionList->addItem(_currentPattern->getOption(i)->toText());
    }
    if (optionListIndex >= _currentPattern->getNumberOfOptions() - 1)
        optionListIndex = _currentPattern->getNumberOfOptions() - 1;
    _ui->patternOptionList->setCurrentRow(optionListIndex);
    updateOptionPanel(optionListIndex);
}

void MainWindow::updateAddOptionButtonActivation(int currentListLine)
{
    if (currentListLine == 0)
        _ui->btnPatternAddOption->setEnabled(false);
    else
        _ui->btnPatternAddOption->setEnabled(true);
}

void MainWindow::updateOptionPanel(int currentListLine)
{
    if (_ui->patternOptionList->count() == 0)
        return;

    if (!_ui->patternOptionList->currentItem())
        return;

    if (_ui->patternOptionList->currentItem()->text().contains(QString("Blink")))
    {
        const Options::Blink * opt = dynamic_cast<Options::Blink const *>(_currentPattern->getOption(currentListLine));
        _ui->optionBlinkX->setValue(opt->getPosition().x());
        _ui->optionBlinkY->setValue(opt->getPosition().y());
        _ui->optionBlinkZ->setValue(opt->getPosition().z());
        _ui->optionBlinkms->setValue(opt->getPeriod());
        _ui->patternOptionPanel->setCurrentIndex(1);
    }
    else if (_ui->patternOptionList->currentItem()->text().contains(QString("Duplicate")))
    {
        const Options::Duplicate * opt = dynamic_cast<Options::Duplicate const *>(_currentPattern->getOption(currentListLine));
        _ui->optionDuplX1->setValue(opt->getVector1().x());
        _ui->optionDuplX2->setValue(opt->getVector2().x());
        _ui->optionDuplX3->setValue(opt->getVector3().x());
        _ui->optionDuplY1->setValue(opt->getVector1().y());
        _ui->optionDuplY2->setValue(opt->getVector2().y());
        _ui->optionDuplY3->setValue(opt->getVector3().y());
        _ui->optionDuplZ1->setValue(opt->getVector1().z());
        _ui->optionDuplZ2->setValue(opt->getVector2().z());
        _ui->optionDuplZ3->setValue(opt->getVector3().z());

        _ui->patternOptionPanel->setCurrentIndex(2);
    }
    else
        ERROR_MSG("Unknown option type: " << _ui->patternOptionList->currentItem()->text());
}

void MainWindow::addOption()
{
    const Options::Type type = (Options::Type)_ui->patternSelectOption->currentIndex();
    _undoStack->push(new Commands::PatternAddOption(this, _currentPattern, type));
}

void MainWindow::deleteOption()
{
    const int index = _ui->patternOptionList->currentRow();
    _undoStack->push(new Commands::PatternDeleteOption(this, _currentPattern, index));
}

void MainWindow::updateOption()
{
    const int index = _ui->patternOptionList->currentRow();

    if (_ui->patternOptionPanel->currentIndex() == 1) //Blink
    {
        Options::Blink opt;
        opt.setPosition(QVector3D(_ui->optionBlinkX->value(), _ui->optionBlinkY->value(), _ui->optionBlinkZ->value()));
        opt.setPeriod(_ui->optionBlinkms->value());
        _undoStack->push(new Commands::PatternUpdateOption(this, _currentPattern, index, opt.toJson()));
    }
    else if (_ui->patternOptionPanel->currentIndex() == 2) //Duplicate
    {
        Options::Duplicate opt;
        opt.setVector1(QVector3D(_ui->optionDuplX1->value(), _ui->optionDuplY1->value(), _ui->optionDuplZ1->value()));
        opt.setVector2(QVector3D(_ui->optionDuplX2->value(), _ui->optionDuplY2->value(), _ui->optionDuplZ2->value()));
        opt.setVector3(QVector3D(_ui->optionDuplX3->value(), _ui->optionDuplY3->value(), _ui->optionDuplZ3->value()));
        _undoStack->push(new Commands::PatternUpdateOption(this, _currentPattern, index, opt.toJson()));
    }
    else
        ERROR_MSG("Uknown option type: " << _ui->patternOptionPanel->currentIndex());
}
