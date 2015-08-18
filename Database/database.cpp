#include "database.h"

Database::Database()
 : _root(new Group("New database"))
{
    //Debug
    Group * g2 = new Group("new group lol");
    Pattern * p = new Pattern("Pattern testttt");
    _root->addChild(g2);
    _root->addChild(p);
    Group * g3 = new Group("woiwowowow");
    Pattern * p2 = new Pattern("much pattern");
    Group * g4 = new Group("such group");
    g2->addChild(g3);
    g2->addChild(g4);
    g3->addChild(p2);

    p->setOn(QVector3D(2,2,2));
    p->setOn(QVector3D(2,4,2));

    Options::Blink * opt = new Options::Blink(QVector3D(1, 3, 4), 500);
    p2->addOption(opt);

    _root->save("/home/pierre/test.txt");
    exit(0);
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
        return static_cast<DbNode*>(getItem(index))->getName();
    }
    else if (role == Qt::DecorationRole)
    {
        return QVariant(QIcon(static_cast<DbNode*>(getItem(index))->getIconPath()));
    }
    else if (role == Qt::EditRole)
    {
        return static_cast<DbNode*>(getItem(index))->getName(); //When the user edits the name, he doesn't want the field to reset!
    }
    else if (role == Qt::UserRole) //Used to store the pointer for drag and drop operations
    {
        DEBUG_MSG("Returned pointer " << getItem(index));
        return QVariant(QString("0x%1").arg((quintptr)getItem(index), QT_POINTER_SIZE * 2, 16, QChar('0')));
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
        static_cast<DbNode*>(getItem(index))->setName(value.toString());
    }
    else if (role == Qt::UserRole) //Set pointer
    {
        DEBUG_MSG("Index has " << getItem(parent(index)) << getItem(parent(index))->getNumberOfChildren() << "children");
        bool ok;
        DbNode * pointer = (DbNode*)(value.toString().toLong(&ok, 16));
        DEBUG_MSG("Got pointer " << pointer);
        DEBUG_MSG("Child index of parent " << getItem(parent(index)) << getItem(parent(index))->getName() << " is " << getItem(index)->getIndex());

        DEBUG_MSG("Index " << index << " node " << getItem(index) << getItem(index)->getName());

        getItem(parent(index))->replaceChild(getItem(index)->getIndex(), pointer);

        DEBUG_MSG("Index " << index << " node " << getItem(index) << getItem(index)->getName());
        DEBUG_MSG("Index has " << getItem(parent(index)) << getItem(parent(index))->getNumberOfChildren() << "children");
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
    DEBUG_MSG("Parent has " << parentItem->getNumberOfChildren() << "children");
    for (int i=0 ; i<count ; i++)
    {
        parentItem->addChild(new Group("new node"), row + i);
    }
    DEBUG_MSG("Parent has " << parentItem->getNumberOfChildren() << "children");
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
    QMimeData * mimeData = new QMimeData();

    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex const & index, indexes)
    {
        if (index.isValid())
        {
            //Add pointer
            QString text = data(index, Qt::UserRole).toString();
            stream << text;
        }
    }

    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool Database::canDropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    if (!data->hasFormat("application/vnd.text.list"))
        return false;

    if (column > 0)
        return false;

    return true;
}

bool Database::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (row == -1)
    {
        row = rowCount(parent);
    }

    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<QVariant> nodesToInsert;

    while (!stream.atEnd())
    {
        QString text;
        stream >> text;
        nodesToInsert.append(QVariant(text));
    }

    int number = 0;
    foreach (QVariant node, nodesToInsert)
    {
        insertRow(row + number, parent);
        QModelIndex idx = index(row + number, 0, parent);
        DEBUG_MSG("Row is " << row << " and idx " << idx);
        setData(idx, node, Qt::UserRole);
        number++;
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
