#ifndef DBNODE_H
#define DBNODE_H

#include "general.h"

class DbNode
{
    public :

        explicit DbNode(QString const & name = QString(""), bool canHaveChildren = true);
        virtual ~DbNode();

        void addChild(DbNode * node, int index = -1);
        void deleteChild(int index);
        void moveChild(int indexBefore, int indexAfter);
        void moveChild(int indexBefore, int indexAfter, DbNode * destinationNode);
        void replaceChild(int index, DbNode * otherNode);
        DbNode * getChild(int index);
        DbNode * getParent();

        void setName(QString const & name);
        QString getName() const;
        Qt::ItemFlags getFlags() const;
        QString getIconPath() const;

        bool canHaveChildren() const;
        int getNumberOfChildren() const;
        int getIndex() const;

        QJsonObject nodeToJson() const;
        virtual QJsonObject toJson() const = 0;
        bool save(QString const & path) const;


    protected :

        QString _name;
        bool _canHaveChildren;

        DbNode* _parent;
        QList<DbNode*> _children;

};

#endif // DBNODE_H
