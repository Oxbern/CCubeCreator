#include "group.h"

Group::Group(QString const & name)
 : DbNode(name, true)
{
    //nothing here
}

QJsonObject Group::toJson() const
{
    //A group Json object is nothing more than a basic node.
    return nodeToJson();
}

//Yeah, I know, that's a quite empty c++ file.
