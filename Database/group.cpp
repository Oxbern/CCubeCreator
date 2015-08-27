#include "group.h"

Group::Group(QString const & name)
 : DbNode(name, true)
{
    //nothing here
}

QJsonObject Group::toJson(bool asRoot) const
{
    //A group Json object is nothing more than a basic node.
    return nodeToJson(asRoot);
}

bool Group::setFromJson(QJsonObject const & json)
{
    //Check type
    if (getObjectType(json) != DbNodeType::Group)
    {
        ERROR_MSG("Cannot load something else than a group in a group object");
        return false;
    }

    //Get name
    if (json["name"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find group name inside database file");
        return false;
    }

    this->_name = json["name"].toString();

    //Get children
    if (json["children"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find children array of group " << this->_name << " inside database file");
        return false;
    }

    QJsonArray children = json["children"].toArray();
    qDeleteAll(_children);
    for (int i=0 ; i<children.size() ; i++)
    {
        //Create child
        const DbNodeType childType = getObjectType(children[i].toObject());
        DbNode * child = nullptr;
        if (childType == DbNodeType::Group)
        {
            child = new Group;
        }
        else if (childType == DbNodeType::Pattern)
        {
            child = new Pattern;
        }
        else
        {
            ERROR_MSG("Uknown type of child " << i << " of group " << this->_name);
            return false;
        }

        //Set child
        if (!child->setFromJson(children[i].toObject()))
        {
            ERROR_MSG("Cannot parse child " << i << " of group " << this->_name);
            return false;
        }

        //Add child
        addChild(child);
    }

    return true;
}
