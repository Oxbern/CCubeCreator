#include "root.h"

Root::Root(QString const & name)
 : Group(name)
{

}

void Root::setModified()
{
    _modified = true;
    emit modified();
}

//This file is small, but it is crucial.
