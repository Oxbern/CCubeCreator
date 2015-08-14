#include "database.h"

Database::Database()
 : _root(new DbNode())
{
    //Debug
    delete _root;
    Group * g1 = new Group("group1");
    _root = g1;
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
    DbNode * parentItem;

    if (parent.isValid())
        parentItem = static_cast<DbNode*>(parent.internalPointer());
    else
        parentItem = _root;

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
    DbNode * parentItem;

    if (parent.column() > 0)
        return 0;

    if (parent.isValid())
        parentItem = static_cast<DbNode*>(parent.internalPointer());
    else
        parentItem = _root;

    return parentItem->getNumberOfChildren();
}

bool Database::hasChildren(const QModelIndex & parent) const
{
    return rowCount(parent) > 0;
}

int Database::columnCount(const QModelIndex & parent) const
{
    (void) parent;
    return 1; //Nodes always have one column (data field).
}

QVariant Database::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    return static_cast<DbNode*>(index.internalPointer())->getName();
}

Qt::ItemFlags Database::flags(const QModelIndex & index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return static_cast<DbNode*>(index.internalPointer())->getFlags();
}

QVariant Database::headerData(int section, Qt::Orientation orientation, int role) const
{
    (void) section; //Useless parameter

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _root->getName();

    return QVariant();
}

//QAbstractItemModel edit
bool Database::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    static_cast<DbNode*>(index.internalPointer())->setName(value.toString());

    return true;
}

bool Database::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    (void) section; //Useless parameter

    if (orientation != Qt::Horizontal || role != Qt::EditRole)
        return false;

    _root->setName(value.toString());

    return true;
}
