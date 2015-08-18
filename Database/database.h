#ifndef DATABASE_H
#define DATABASE_H

#include "general.h"

#include "dbnode.h"
#include "group.h"
#include "pattern.h"

class Database : public QAbstractItemModel
{
    public :

        explicit Database();
        virtual ~Database() override;

        //QAbstractItemModel basics
        virtual QModelIndex         index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;
        virtual QModelIndex         parent(const QModelIndex & index) const override;
        virtual int                 rowCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual bool                hasChildren(const QModelIndex & parent = QModelIndex()) const override;
        virtual int                 columnCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual QVariant            data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
        virtual Qt::ItemFlags       flags(const QModelIndex & index) const override;
        virtual QVariant            headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        //QAbstractItemModel edit
        virtual bool                setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
        virtual bool                setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole) override;
        virtual bool                insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
        virtual bool                moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild) override;
        virtual bool                removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

        //QAbstractItemModel drag and drop
        virtual Qt::DropActions 	supportedDragActions() const override;
        virtual Qt::DropActions 	supportedDropActions() const override;
        virtual QStringList         mimeTypes() const override;
        virtual QMimeData *         mimeData(const QModelIndexList & indexes) const override;
        virtual bool                canDropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) const override;
        virtual bool                dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) override;

/*        virtual QModelIndex         buddy(const QModelIndex & index) const override;
        virtual bool                canFetchMore(const QModelIndex & parent) const override;
        virtual void                fetchMore(const QModelIndex & parent) override;
        virtual bool                insertColumns(int column, int count, const QModelIndex & parent = QModelIndex()) override;
        virtual QMap<int, QVariant> itemData(const QModelIndex & index) override;
        virtual QModelIndexList 	match(const QModelIndex & start, int role, const QVariant & value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap )) override;
        virtual bool                moveColumns(const QModelIndex & sourceParent, int sourceColumn, int count, const QModelIndex & destinationParent, int destinationChild) override;
        virtual bool                removeColumns(int column, int count, const QModelIndex & parent = QModelIndex()) override;
        virtual QHash<int, QByteArray> 	roleNames() const override;
        virtual bool                setItemData(const QModelIndex & index, const QMap<int, QVariant> & roles) override;
        virtual QModelIndex         sibling(int row, int column, const QModelIndex & index) const override;
        virtual void                sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
        virtual QSize               span(const QModelIndex & index) const override;
*/

        //Custom useful additions to the model class
        DbNode * getItem(QModelIndex const & index) const;

        //Specific stuff for the CCube
        //void


    private :

        DbNode* _root;


    public slots :

        //QAbstractItemModel
        //virtual void revert() override;
        //virtual bool submit() override;

};

#endif // DATABASE_H
