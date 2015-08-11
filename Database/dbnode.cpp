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

void DbNode::addChild(DbNode * node)
{
    _children.append(node);
}

DbNode * DbNode::getChild(int index)
{
    return _children.value(index);
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
