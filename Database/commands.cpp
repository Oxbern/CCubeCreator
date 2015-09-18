#include "commands.h"

using namespace Commands;

#include "database.h"
#include "mainwindow.h"

/********************/
/* Tree index class */
/********************/

TreeIndex::TreeIndex(QModelIndex const & index, Database * db)
 : _db(db)
{
    _row = index.row();
    _column = index.column();
    if (index.isValid())
    {
        _parent.reset(new TreeIndex(index.parent(), db));
    }
    else //is root
    {
        _parent.clear();
    }
}

TreeIndex::TreeIndex(int row, QModelIndex const & parent, Database * db)
 : _db(db)
{
   _row = row;
   _column = 0;
   if (parent.isValid())
   {
       _parent.reset(new TreeIndex(parent, db));
   }
   else
   {
       _parent.clear();
   }
}

QModelIndex TreeIndex::get() const
{
    if (_parent.isNull()) //is root
    {
        return QModelIndex();
    }
    else
    {
        return _db->index(_row, _column, _parent->get());
    }
}

int TreeIndex::row() const
{
    return _row;
}

QModelIndex TreeIndex::parent() const
{
    if (_parent.isNull())
    {
        return QModelIndex();
    }
    else
        return _parent->get();
}

/*********************************************/
/* Generic command for database manipulation */
/*********************************************/

//Useful to make other commands faster

GenericDbCommand::GenericDbCommand(QModelIndex const & index, QVariant const & data, Database * db)
 : QUndoCommand(), _db(db), _new(data)
{
    _index = TreeIndex(index, db);
    _wasModified = db->getItem(index)->wasModified();
}

void GenericDbCommand::redo()
{
    action(_new);
    _db->getItem(_index.get())->setModified();
}

void GenericDbCommand::undo()
{
    action(_old);
    if (_wasModified)
        _db->getItem(_index.get())->setModified();
    else
        _db->getItem(_index.get())->setUnmodified();
}

/**********/
/* Rename */
/**********/

RenameNode::RenameNode(QModelIndex const & index, QVariant const & name, Database * db)
 : GenericDbCommand(index, name, db)
{
    _old = index.data(Qt::EditRole);
    setText(QObject::tr("Renamed %1 to %2").arg(_old.toString()).arg(_new.toString()));
}

void RenameNode::action(QVariant const & data)
{
    QModelIndex index = _index.get();

    static_cast<DbNode*>(_db->getItem(index))->setName(data.toString());
    _db->emitDataChanged(index);
}

/*************************/
/* Set content from Json */
/*************************/

NodeSetContentFromJson::NodeSetContentFromJson(QModelIndex const & index, QVariant const & json, Database * db)
 : GenericDbCommand(index, json, db)
{
    _old = index.data(Qt::ContentRole);
    setText(QObject::tr("Set content of %1 from json object").arg(index.data(Qt::EditRole).toString()));
}

void NodeSetContentFromJson::action(QVariant const & data)
{
    QModelIndex index = _index.get();
    static_cast<DbNode*>(_db->getItem(index))->setFromJson(data.toJsonObject());
    _db->emitDataChanged(index);
}

/*****************/
/* Set node type */
/*****************/

SetNodeType::SetNodeType(QModelIndex const & index, QVariant const & type, Database * db)
 : GenericDbCommand(index, type, db)
{
    _old = index.data(Qt::TypeRole);
    setText(QObject::tr("Set type of %1 to %2").arg(index.data(Qt::EditRole).toString()).arg(type.toInt()));
}

void SetNodeType::action(QVariant const & data)
{
    QModelIndex index = _index.get();

    DbNode * node = static_cast<DbNode*>(_db->getItem(index));
    const DbNodeType currentType = node->getType();
    QString nodeName = node->getName();
    const DbNodeType newType = (DbNodeType)data.toInt();

    if (currentType != newType)
    {
        if (newType == DbNodeType::Group)
        {
            node = new Group(nodeName);
        }
        else if (newType == DbNodeType::Pattern)
        {
            if (nodeName == QString("New group"))
                nodeName = QString("New pattern");
            node = new Pattern(nodeName);
        }
        else
            ERROR_MSG("Cannot change node type of node " << nodeName << ": unknown type");

        _db->emitDataChanged(index);
    }
}

/***************/
/* Insert node */
/***************/

InsertNode::InsertNode(QModelIndex const & index, QVariant const & type, Database * db)
 : GenericDbCommand(index, type, db)
{
    _old = QVariant(); //Don't care
    _type = (DbNodeType)type.toInt();
    const QString parentName = db->getItem(index)->getName();

    if (_type == DbNodeType::Group)
    {
        setText(QObject::tr("Insert new group to %1").arg(parentName));
    }
    else if (_type == DbNodeType::Pattern)
    {
        setText(QObject::tr("Insert new pattern in %1").arg(parentName));
    }
    else
        ERROR_MSG("Cannot add node of unkown type to " << parentName);
}

void InsertNode::redo()
{
    QModelIndex index = _index.get();

    _insertedRowNumber = _db->rowCount(index) + 1;
    _db->insertRowsType(_type, _insertedRowNumber, 1, index);

    _db->getItem(_index.get())->setModified();
}

void InsertNode::undo()
{
    _db->removeRows(_insertedRowNumber, 1, _index.get());

    if (_wasModified)
        _db->getItem(_index.get())->setModified();
    else
        _db->getItem(_index.get())->setUnmodified();
}

/***************/
/* Remove node */
/***************/

RemoveNode::RemoveNode(QModelIndex const & index, QVariant const & json, Database * db)
 : GenericDbCommand(index, json, db)
{
    _old = QVariant(); //Don't care
    setText(QObject::tr("Remove %1").arg(_db->getItem(index)->getName()));
}

void RemoveNode::redo()
{
    QModelIndex index = _index.get();
    _db->removeRows(index.row(), 1, index.parent());

    _db->getItem(_index.get())->setModified();
}

void RemoveNode::undo()
{
    const QJsonObject json = _new.toJsonObject();
    const DbNodeType type = DbNode::getObjectType(json);

    _db->insertRowsType(type, _index.row(), 1, _index.parent());
    QModelIndex index = _index.get(); //We need to insert the row before getting the index!

    _db->getItem(index)->setFromJson(json);

    if (_wasModified)
        _db->getItem(index)->setModified();
    else
        _db->getItem(index)->setUnmodified();
}

/*************/
/* Move node */
/*************/

MoveNode::MoveNode(QModelIndex const & source, int destinationRow, QModelIndex const & destinationParent, QJsonObject const & data, Database * db, bool fromDragAndDrop)
 : QUndoCommand(), _source(source, db), _destination(destinationRow, destinationParent, db), _data(data), _db(db), _dragAndDrop(fromDragAndDrop)
{
    setText(QObject::tr("Move %1 from (%2, %3) to (%4, %5)")
            .arg(_db->getItem(source)->getName())
            .arg(_db->getItem(source.parent())->getName())
            .arg(source.row())
            .arg(_db->getItem(destinationParent)->getName())
            .arg(destinationRow));
    _isModified = db->getItem(source)->wasModified();
}

void MoveNode::redo()
{
    //Add destination
    const DbNodeType type = DbNode::getObjectType(_data);

    _db->insertRowsType(type, _destination.row(), 1, _destination.parent());
    QModelIndex index = _destination.get();

    _db->getItem(index)->setFromJson(_data);
    //_db->getItem(index)->setName("name");

    if (_isModified)
        _db->getItem(index)->setModified();
    else
        _db->getItem(index)->setUnmodified();

    _db->makeViewSelectIndex(_destination.get());

    //If needed, remove source
    if (!_dragAndDrop) //In drag and drop mode, Qt removes the source by itself
    {
        int row = _destination.row() < _source.row() ? _source.row() + 1 : _source.row();
        _db->removeRows(row, 1, _source.parent());
    }
}

void MoveNode::undo()
{
    //Remove destination
    int removeRow = _destination.row() > _source.row() ? _destination.row() - 1 : _destination.row();
    _db->removeRows(removeRow, 1, _destination.parent());

    //Add source
    const DbNodeType type = DbNode::getObjectType(_data);

    _db->insertRowsType(type, _source.row(), 1, _source.parent());
    QModelIndex index = _source.get();

    _db->getItem(index)->setFromJson(_data);

    if (_isModified)
        _db->getItem(index)->setModified();
    else
        _db->getItem(index)->setUnmodified();

    _db->makeViewSelectIndex(_source.get());
}

/******************/
/* Reset database */
/******************/

ClearDatabase::ClearDatabase(Database * db)
 : QUndoCommand(), _db(db)
{
    _oldRoot = _db->_root;
    _newRoot = new Root("New database");
    setText(QObject::tr("Reset database"));
}

ClearDatabase::~ClearDatabase()
{
    delete _oldRoot;
}

void ClearDatabase::redo()
{
    _db->_root = _newRoot;
    _db->connectRoot();
}

void ClearDatabase::undo()
{
    _db->_root = _oldRoot;
    _db->connectRoot();
}

/***************************/
/* Generic pattern command */
/***************************/

//All pattern commands share this simple lazy design:
//We store the pattern inside _old, execute the action that modifies the pattern
//and then store the new pattern inside _new.

GenericPatternCommand::GenericPatternCommand(MainWindow * view, Pattern * pattern)
 : _view(view), _pattern(pattern)
{
    _wasModified = pattern->wasModified();
}

void GenericPatternCommand::init()
{
    _old = _pattern->toJson();
    action();
    _new = _pattern->toJson();
}

void GenericPatternCommand::redo()
{
    _pattern->setFromJson(_new);
    _view->updatePatternContent();
    _pattern->setModified();
}

void GenericPatternCommand::undo()
{
    _pattern->setFromJson(_old);
    _view->updatePatternContent();
    if (_wasModified)
        _pattern->setModified();
    else
        _pattern->setUnmodified();
}

/*************************/
/* Add option to pattern */
/*************************/

PatternAddOption::PatternAddOption(MainWindow * view, Pattern * pattern, Options::Type type)
 : GenericPatternCommand(view, pattern), _type(type)
{
    init();
    setText(QObject::tr("Add option of type %1 to %2").arg((int)type).arg(pattern->getName()));
}

void PatternAddOption::action()
{
    Options::Option * opt = nullptr;

    if (_type == Options::Type::Blink)
    {
        opt = new Options::Blink();
    }
    else if (_type == Options::Type::Duplicate)
    {
        opt = new Options::Duplicate();
    }
    else
    {
        ERROR_MSG("Cannot add new option: unknown type " << (int)_type);
    }

    _pattern->addOption(opt);
}

/*****************/
/* Delete option */
/*****************/

PatternDeleteOption::PatternDeleteOption(MainWindow * view, Pattern * pattern, int index)
 : GenericPatternCommand(view, pattern), _index(index)
{
    init();
    setText(QObject::tr("Delete option %1 from %2").arg(index).arg(_pattern->getName()));
}

void PatternDeleteOption::action()
{
    _pattern->removeOption(_index);
}

/*****************/
/* Update option */
/*****************/

PatternUpdateOption::PatternUpdateOption(MainWindow * view, Pattern * pattern, int index, QJsonObject const & json)
 : GenericPatternCommand(view, pattern), _index(index), _json(json)
{
    init();
    setText(QObject::tr("Update option %1 of %2").arg(index).arg(_pattern->getName()));
}

void PatternUpdateOption::action()
{
    _pattern->setOption(_index, _json);
}

/*********************/
/* Toggle single LED */
/*********************/

PatternToggleLED::PatternToggleLED(MainWindow * view, Pattern * pattern, QVector3D const & ledCoordinates)
 : GenericPatternCommand(view, pattern), _led(ledCoordinates)
{
    init();
    setText(QObject::tr("Toggle LED (%1, %2, %3) of %4").arg(ledCoordinates.x()).arg(ledCoordinates.y()).arg(ledCoordinates.z()).arg(_pattern->getName()));
}

void PatternToggleLED::action()
{
    _pattern->toggle(_led);
}

/**********************/
/* Change description */
/**********************/

PatternChangeDescription::PatternChangeDescription(MainWindow * view, Pattern * pattern, QString const & text)
 : GenericPatternCommand(view, pattern), _text(text)
{
    init();
    setText(QObject::tr("Change description of %1").arg(_pattern->getName()));
}

void PatternChangeDescription::action()
{
    _pattern->setDescription(_text);
}

/****************/
/* Change image */
/****************/

PatternChangeImage::PatternChangeImage(MainWindow * view, Pattern * pattern, QImage const & img)
 : GenericPatternCommand(view, pattern), _image(img)
{
    init();
    setText(QObject::tr("Change thumbnail of %1").arg(_pattern->getName()));
}

void PatternChangeImage::action()
{
    _pattern->setImage(_image);
}
