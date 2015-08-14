#ifndef DBNODE_H
#define DBNODE_H

#include "general.h"

class DbNode
{
    public :

        explicit DbNode(QString const & name = QString(""), bool canHaveChildren = true);
        virtual ~DbNode();

        void addChild(DbNode * node);
        DbNode * getChild(int index);
        DbNode * getParent();

        void setName(QString const & name);
        QString getName() const;

        bool canHaveChildren() const;
        int getNumberOfChildren() const;
        int getIndex() const;
        Qt::ItemFlags getFlags() const;


    protected :

        bool _canHaveChildren;
        QString _name;

        DbNode* _parent;
        QList<DbNode*> _children;

};

#endif // DBNODE_H
