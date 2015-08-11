#ifndef DATABASE_H
#define DATABASE_H

#include "general.h"

#include "group.h"

class Database
{
    public :

        explicit Database();


    private :

        QList<Group> groups;

};

#endif // DATABASE_H
