#include "database.h"

Database::Database(QUndoStack * undoStack)
 : _root(new Root("New database")), _undoStack(undoStack)
{
    connectRoot();
    _dragAndDropSourceIndexes.clear();
}

Database::~Database()
{
    delete _root;
}

//QAbstractItemModel basics

QModelIndex Database::index(int row, int column, const QModelIndex & parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    //Parent
    DbNode * parentItem = getItem(parent);

    //Child
    DbNode * childItem = parentItem->getChild(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex Database::parent(const QModelIndex & index) const
{
    if (!index.isValid())
        return QModelIndex();

    DbNode * childItem = static_cast<DbNode*>(index.internalPointer());
    DbNode * parentItem = childItem->getParent();

    if (parentItem == _root)
        return QModelIndex();

    return createIndex(parentItem->getIndex(), 0, parentItem);
}

int Database::rowCount(const QModelIndex & parent) const
{
    return getItem(parent)->getNumberOfChildren();
}

bool Database::hasChildren(const QModelIndex & parent) const
{
    return rowCount(parent) > 0;
}

int Database::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 1; //Nodes always have one column (data field).
}

QVariant Database::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const QString suffix = static_cast<DbNode*>(getItem(index))->wasModified() ? QString(" *") : QString("");
        return static_cast<DbNode*>(getItem(index))->getName() + suffix;
    }
    else if (role == Qt::DecorationRole)
    {
        return QVariant(QIcon(static_cast<DbNode*>(getItem(index))->getIconPath()));
    }
    else if (role == Qt::EditRole)
    {
        return static_cast<DbNode*>(getItem(index))->getName(); //When the user edits the name, he doesn't want the field to reset!
    }
    else if (role == Qt::ContentRole) //json
    {
        return QVariant(getItem(index)->toJson());
    }
    else if (role == Qt::TypeRole) //type
    {
        return QVariant((int)getItem(index)->getType());
    }
    else
        return QVariant();
}

Qt::ItemFlags Database::flags(const QModelIndex & index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return static_cast<DbNode*>(index.internalPointer())->getFlags();
}

QVariant Database::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _root->getName();

    return QVariant();
}

//QAbstractItemModel edit

bool Database::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid())
    {
        DEBUG_MSG("Index invalid");
        return false;
    }

    if (role == Qt::EditRole) //Set name
    {
        _undoStack->push(new Commands::RenameNode(index, value, this));
    }
    else if (role == Qt::ContentRole) //Set content from json
    {
        _undoStack->push(new Commands::NodeSetContentFromJson(index, value, this));
    }
    else if (role == Qt::TypeRole) //Set node type
    {
        _undoStack->push(new Commands::SetNodeType(index, value, this));
    }
    else
    {
        return false;
    }

    return true;
}

bool Database::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    Q_UNUSED(section);

    if (orientation != Qt::Horizontal || role != Qt::EditRole)
        return false;

    _root->setName(value.toString());

    return true;
}

bool Database::insertRows(int row, int count, const QModelIndex & parent)
{
    beginInsertRows(parent, row, row + count - 1);

    DbNode * parentItem = getItem(parent);

    for (int i=0 ; i<count ; i++)
    {
        parentItem->addChild(new Group("New group"), row + i);
    }

    endInsertRows();

    return true;
}

bool Database::moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild)
{
    beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

    DbNode * sourceNode = getItem(sourceParent);
    DbNode * destinationNode = getItem(destinationParent);
    for (int i=0 ; i<count ; i++)
    {
        sourceNode->moveChild(sourceRow, destinationChild + i, destinationNode);
    }

    endMoveRows();

    return true;
}

bool Database::removeRows(int row, int count, const QModelIndex & parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    DbNode * parentItem = getItem(parent);
    for (int i=0 ; i<count ; i++)
    {
        parentItem->deleteChild(row);
    }

    endRemoveRows();

    return true;
}


//QAbstractItemModel drag and drop

Qt::DropActions Database::supportedDragActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions Database::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList Database::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}

QMimeData * Database::mimeData(const QModelIndexList & indexes) const
{
    QList<QJsonObject> json;

    foreach (QModelIndex const & index, indexes)
    {
        if (index.isValid())
            //Insert node json inside mime
            json.append(data(index, Qt::ContentRole).toJsonObject());
    }

    FakeMimeData * mimeData = new FakeMimeData(indexes, json);

    return mimeData;
}

bool Database::canDropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    //Check mime type
    if (data->text() != QString("Oxbern::CCube"))
        return false;

    //Check action type
    if (action != Qt::MoveAction)
        return false;

    //Check destination (must not be a pattern)
    QModelIndex destination = parent; //Weird shit

    if (getItem(destination)->getType() != DbNodeType::Group)
        return false;

    return true;
}

bool Database::dropMimeData(const QMimeData * mime, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
    if (!canDropMimeData(mime, action, row, column, parent))
        return false;

    if (row == -1)
    {
        row = rowCount(parent);
    }

    const FakeMimeData * fakeMime = dynamic_cast<FakeMimeData const *>(mime);
    for (int i=0 ; i<fakeMime->size() ; i++)
    {
        _undoStack->push(new Commands::MoveNode(fakeMime->indexes()[i], row + i, parent, fakeMime->data()[i], this, true));
    }

    return true;
}

DbNode * Database::getItem(QModelIndex const & index) const
{
    if (index.isValid())
    {
        DbNode *item = static_cast<DbNode*>(index.internalPointer());
        if (item)
            return item;
    }
    return _root;
}

bool Database::insertRowsType(DbNodeType type, int row, int count, const QModelIndex & parent)
{
    beginInsertRows(parent, row, row + count - 1);

    DbNode * parentItem = getItem(parent);

    if (type == DbNodeType::Group)
    {
        for (int i=0 ; i<count ; i++)
            parentItem->addChild(new Group("New group"), row + i);
    }
    else if (type == DbNodeType::Pattern)
    {
        for (int i=0 ; i<count ; i++)
            parentItem->addChild(new Pattern("New pattern"), row + i);
    }
    else
    {
        ERROR_MSG("Cannot insert unknown object");
    }

    endInsertRows();

    return true;
}

void Database::emitDataChanged(QModelIndex const & index)
{
    emit dataChanged(index, index);
}

void Database::reset()
{
    beginResetModel();

    _undoStack->push(new Commands::ClearDatabase(this));

    endResetModel();
}

void Database::setUnmodified()
{
    _root->setUnmodified();
}

QString Database::getRootName() const
{
    return _root->getName();
}

void Database::setRootName(QString const & newName)
{
    _root->setName(newName);
}

bool Database::save(QString const & path)
{
    return _root->save(path);
}

bool Database::load(QString const & path)
{
    const bool result = _root->load(path);
    emit layoutChanged();
    return result;
}

void Database::connectRoot()
{
    connect(_root, SIGNAL(modified()), this, SIGNAL(modified()));
}

void Database::makeViewSelectIndex(QModelIndex const & index)
{
    emit select(index);
}
