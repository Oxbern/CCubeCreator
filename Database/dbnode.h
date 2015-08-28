#ifndef DBNODE_H
#define DBNODE_H

#include "general.h"

enum class DbNodeType
{
    Group,
    Pattern,
    Unknown
};

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

        QJsonObject nodeToJson(bool asRoot = false) const;
        virtual QJsonObject toJson(bool asRoot = false) const = 0;
        bool save(QString const & path) const;

        static DbNodeType getObjectType(QJsonObject const & json);
        virtual bool setFromJson(QJsonObject const & json) = 0;
        bool load(QString const & path);

        virtual DbNodeType getType() const = 0;


    protected :

        QString _name;
        bool _canHaveChildren;

        DbNode* _parent;
        QList<DbNode*> _children;

};

#endif // DBNODE_H
