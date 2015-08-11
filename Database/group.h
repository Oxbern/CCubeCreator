#ifndef GROUP_H
#define GROUP_H

#include "general.h"

#include "dbnode.h"
#include "pattern.h"

class Group : public DbNode
{
    public :

        explicit Group(QString const & name = QString(""));

};

#endif // GROUP_H
