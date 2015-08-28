#ifndef GROUP_H
#define GROUP_H

#include "general.h"

#include "dbnode.h"
#include "pattern.h"

class Group : public DbNode
{
    public :

        explicit Group(QString const & name = QString(""));

        virtual QJsonObject toJson(bool asRoot = false) const override;
        virtual bool setFromJson(QJsonObject const & json) override;

        virtual DbNodeType getType() const override;


};

#endif // GROUP_H
