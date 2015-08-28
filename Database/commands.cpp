#include "commands.h"

using namespace Commands;

#include "database.h"

/**********/
/* Rename */
/**********/

RenameNode::RenameNode(QModelIndex const & index, QString const & name, Database * db)
 : QUndoCommand(), _db(db)
{
    _old = index.data().toString();
    _new = name;
    _index = index;
    setText(QObject::tr("Renamed %1 to %2").arg(_old).arg(_new));
}

void RenameNode::redo()
{
    static_cast<DbNode*>(_db->getItem(_index))->setName(_new);
    _db->emitDataChanged(_index);
    DEBUG_MSG("redid name change");
}

void RenameNode::undo()
{
    static_cast<DbNode*>(_db->getItem(_index))->setName(_old);
    _db->emitDataChanged(_index);
    DEBUG_MSG("undid");
}
