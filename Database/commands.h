#ifndef COMMANDS_H
#define COMMANDS_H

#include "general.h"

#include "dbnode.h"

class Database;

namespace Commands
{

class RenameNode : public QUndoCommand
{
    public :

        RenameNode(QModelIndex const & index, QString const & name, Database * db);

        virtual void redo() override;
        virtual void undo() override;


    private :

        Database * _db;
        QString _old, _new;
        QModelIndex _index;
};


       /* QAction * _actionAddGroup;
        QAction * _actionAddPattern;
        QAction * _actionDeleteNode;
        QAction * _actionMoveNode;
        QAction * _actionRenameNode;

        QAction * _actionSetLeds;
        QAction * _actionChangeImagePath;
        QAction * _actionChangeDescription;

        QAction * _actionAddOption;
        QAction * _actionDeleteOption;
        QAction * _actionChangeOption;*/

} //namespace

#endif // COMMANDS_H
