#include "dbnode.h"

DbNode::DbNode(QString const & name, bool canHaveChildren)
 : _name(name), _canHaveChildren(canHaveChildren)
{
    _parent = nullptr;
    _children.clear();
}

DbNode::~DbNode()
{
    qDeleteAll(_children);
}

void DbNode::replaceChild(int index, DbNode * otherNode)
{
    DEBUG_MSG("Current element " << this->getName() << " has " << this->getNumberOfChildren() << " children");
    addChild(otherNode, index);
    DEBUG_MSG("Current element " << this->getName() << " has " << this->getNumberOfChildren() << " children");
    deleteChild(index + 1);
    DEBUG_MSG("Current element " << this->getName() << " has " << this->getNumberOfChildren() << " children");

    /*if (index >= 0 && index < _children.size())
    {
        DbNode * save = _children.value(index);

        _children

    }*/
}

void DbNode::addChild(DbNode * node, int index)
{
    if (index >= 0 && index < _children.size())
    {
        _children.insert(index, node);
        _children[index]->_parent = this;
    }
    else
    {
        _children.append(node);
        _children.back()->_parent = this;
    }
}

void DbNode::deleteChild(int index)
{
    if (index >= 0 && index < _children.size())
    {
        delete _children.value(index);
        _children.removeAt(index);
    }
}

void DbNode::moveChild(int indexBefore, int indexAfter)
{
    if (indexBefore >= 0 && indexBefore < _children.size())
    {
        DbNode * tmp = this->_children.value(indexBefore);
        this->_children.removeAt(indexBefore);

        this->addChild(tmp, indexAfter);
    }
}

void DbNode::moveChild(int indexBefore, int indexAfter, DbNode * destinationNode)
{
    if (indexBefore >= 0 && indexBefore < _children.size())
    {
        DbNode * tmp = this->_children.value(indexBefore);
        this->_children.removeAt(indexBefore);

        destinationNode->addChild(tmp, indexAfter);
    }
}

DbNode * DbNode::getChild(int index)
{
    return index >= 0 && index < _children.size() ? _children.value(index) : nullptr;
}

DbNode * DbNode::getParent()
{
    return _parent;
}

void DbNode::setName(QString const & name)
{
    _name = name;
}

QString DbNode::getName() const
{
    return _name;
}

Qt::ItemFlags DbNode::getFlags() const
{
    Qt::ItemFlags flags =
            Qt::ItemIsSelectable |
            Qt::ItemIsEditable |
            Qt::ItemIsEnabled |
            Qt::ItemIsDragEnabled |
            Qt::ItemIsDropEnabled;

    if (!canHaveChildren())
        flags |= Qt::ItemNeverHasChildren;

    return flags;
}

QString DbNode::getIconPath() const
{
    if (canHaveChildren())
        return QString(":/icons/Icons/SnowIsh-1.0_PNG/128x128/actions/edit-clear.png");
    else
        return QString(":/icons/Icons/SnowIsh-1.0_PNG/128x128/actions/edit-redo.png");
}

bool DbNode::canHaveChildren() const
{
    return _canHaveChildren;
}

int DbNode::getNumberOfChildren() const
{
    return _children.size();
}

int DbNode::getIndex() const
{
    return _parent->_children.indexOf(const_cast<DbNode*>(this));
}

QJsonObject DbNode::nodeToJson() const
{
    QJsonObject node;

    //Name
    node["name"] = _name;

    //Type
    if (_canHaveChildren)
    {
        node["type"] = QString("group");
    }
    else
    {
        node["type"] = QString("pattern");
    }

    //Children (if any)
    QJsonArray children;
    if (_canHaveChildren)
    {
        for (DbNode * n : _children)
        {
            children.append(n->toJson());
        }
        node["children"] = children;
    }

    return node;
}

bool DbNode::save(QString const & path) const
{
    QJsonObject object = this->toJson();
    QJsonDocument doc(object);

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly))
    {
        ERROR_MSG("Cannot open file (w): " << path);
        return false;
    }

    file.write(doc.toJson());

    return true;
}
