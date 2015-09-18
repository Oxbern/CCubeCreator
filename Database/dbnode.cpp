#include "dbnode.h"

DbNode::DbNode(QString const & name, bool canHaveChildren)
 : _name(name), _canHaveChildren(canHaveChildren)
{
    _parent = nullptr;
    _children.clear();
    setUnmodified();
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
        return QString(":/icons/Icons/ccdb.png");
    else
        return QString(":/icons/Icons/ccpat.png");
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

QJsonObject DbNode::nodeToJson(bool asRoot) const
{
    QJsonObject node;

    //Name
    node["name"] = _name;

    //Version (if root)
    if (asRoot)
        node["version"] = QString::number((double)CCUBE_DDB_VERSION);

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
    QJsonObject object = this->toJson(true);
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

DbNodeType DbNode::getObjectType(QJsonObject const & json)
{
    if (json["type"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find object type: database/pattern file is probably corrupted");
        return DbNodeType::Unknown;
    }

    if (json["type"].toString() == QString("group"))
    {
        return DbNodeType::Group;
    }
    else if (json["type"].toString() == QString("pattern"))
    {
        return DbNodeType::Pattern;
    }
    else
        return DbNodeType::Unknown;
}

bool DbNode::load(QString const & path)
{
    /* Note:
     * Before calling this function, you need to make sure you're calling it
     * from a group if the file is a database (.ccdb)
     * or that you're calling it from a pattern if the file a single pattern (.ccpat).
     */

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        ERROR_MSG("Cannot open file (r): " << path);
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));

    //Verify version
    if (doc.object()["version"] == QJsonValue::Undefined)
    {
        ERROR_MSG("The file " << path << " doesn't contain a version number");
        return false;
    }
    const float version = doc.object()["version"].toString().toFloat();
    if (version > CCUBE_DDB_VERSION)
    {
        ERROR_MSG("The file " << path << " uses a newer database version. Please upgrade CCube Creator.");
    }

    //Call parse function
    return setFromJson(doc.object());
}

void DbNode::setModified()
{
    _modified = true;
    _parent->setModified();
}

void DbNode::setUnmodified()
{
    _modified = false;
    for (int i=0 ; i<_children.size() ; i++)
        _children[i]->setUnmodified();
}

bool DbNode::wasModified() const
{
    return _modified;
}
