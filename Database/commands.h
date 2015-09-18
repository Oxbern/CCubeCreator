#ifndef COMMANDS_H
#define COMMANDS_H

#include "general.h"

#include "dbnode.h"
#include "pattern.h"
#include "root.h"

class Database;
class MainWindow;

namespace Commands
{

class TreeIndex //A useful class to keep an element position inside a tree.
{
    /* QModelIndex changes if the pointer changes, which can happen with undo/redo if we reinsert a node for example.
     * Hence, we need a class able to store an index that only depends on row numbers of successive nodes.
     * NB: even QPersistentModelIndex isn't enough (for some reason).
     */

    public :

        explicit TreeIndex(QModelIndex const & index = QModelIndex(), Database * db = nullptr);
        explicit TreeIndex(int row, QModelIndex const & parent, Database * db);
        QModelIndex get() const;

        int row() const;
        QModelIndex parent() const;

    private :

        QSharedPointer<TreeIndex> _parent;
        int _row, _column;
        Database * _db;
};

class GenericDbCommand : public QUndoCommand
{
    public :

        explicit GenericDbCommand(QModelIndex const & index, QVariant const & data, Database * db);

        virtual void redo() override;
        virtual void undo() override;
        virtual void action(QVariant const & data) = 0;


    protected :

        Database * _db;
        QVariant _old, _new;
        TreeIndex _index;
        bool _wasModified;
};

struct RenameNode : public GenericDbCommand
{
    explicit RenameNode(QModelIndex const & index, QVariant const & name, Database * db);
    virtual void action(QVariant const & data) override;
};

struct NodeSetContentFromJson : public GenericDbCommand
{
    explicit NodeSetContentFromJson(QModelIndex const & index, QVariant const & json, Database * db);
    virtual void action(QVariant const & data) override;
};

struct SetNodeType : public GenericDbCommand
{
    explicit SetNodeType(QModelIndex const & index, QVariant const & type, Database * db);
    virtual void action(QVariant const & data) override;
};

class InsertNode : public GenericDbCommand //This one's a bit special
{
    public :

        explicit InsertNode(QModelIndex const & parent, QVariant const & type, Database * db);

        virtual void redo() override;
        virtual void undo() override;
        virtual void action(QVariant const & data) override {Q_UNUSED(data);}

    private :

        int _insertedRowNumber;
        DbNodeType _type;
};

class RemoveNode : public GenericDbCommand //This one also
{
    public :

        explicit RemoveNode(QModelIndex const & parent, QVariant const & json, Database * db);

        virtual void redo() override;
        virtual void undo() override;
        virtual void action(QVariant const & data) override {Q_UNUSED(data);}
};

class MoveNode : public QUndoCommand //This one is too different to inherit from GenericDbCommand
{
    public :

        explicit MoveNode(QModelIndex const & source, int destinationRow, QModelIndex const & destinationParent, QJsonObject const & data, Database * db, bool fromDragAndDrop = false);

        virtual void redo() override;
        virtual void undo() override;


    protected :

        TreeIndex _source, _destination;
        QJsonObject _data;
        Database * _db;
        bool _dragAndDrop;
        bool _isModified;
};

class ClearDatabase : public QUndoCommand
{
    public :

        explicit ClearDatabase(Database * db);
        ~ClearDatabase();

        virtual void redo() override;
        virtual void undo() override;


    protected :

        Database * _db;
        Root * _oldRoot;
        Root * _newRoot;
};

class GenericPatternCommand : public QUndoCommand
{
    public :

        explicit GenericPatternCommand(MainWindow * view, Pattern * pattern);

        virtual void redo() override;
        virtual void undo() override;

        void init(); //You need to call this in your subclass' constructor (it uses a pure virtual function and hence cannot be called from GenericPatternCommand constructor).
        virtual void action() = 0;


    protected :

        QJsonObject _old, _new;
        MainWindow * _view;
        Pattern * _pattern;
        bool _wasModified;
};

struct PatternAddOption : public GenericPatternCommand
{
    explicit PatternAddOption(MainWindow * view, Pattern * pattern, Options::Type type);
    virtual void action() override;
    private :
    Options::Type _type;
};

struct PatternDeleteOption : public GenericPatternCommand
{
    explicit PatternDeleteOption(MainWindow * view, Pattern * pattern, int index);
    virtual void action() override;
    private :
    int _index;
};

struct PatternUpdateOption : public GenericPatternCommand
{
    explicit PatternUpdateOption(MainWindow * view, Pattern * pattern, int index, QJsonObject const & json);
    virtual void action() override;
    private :
    int _index;
    QJsonObject _json;
};

struct PatternToggleLED : public GenericPatternCommand
{
    explicit PatternToggleLED(MainWindow * view, Pattern * pattern, QVector3D const & ledCoordinates);
    virtual void action() override;
    private :
    QVector3D _led;
};

struct PatternChangeDescription : public GenericPatternCommand
{
    explicit PatternChangeDescription(MainWindow * view, Pattern * pattern, QString const & text);
    virtual void action() override;
    private :
    QString _text;
};

struct PatternChangeImage : public GenericPatternCommand
{
    explicit PatternChangeImage(MainWindow * view, Pattern * pattern, QImage const & img);
    virtual void action() override;
    private :
    QImage _image;
};

} //namespace

#endif // COMMANDS_H
