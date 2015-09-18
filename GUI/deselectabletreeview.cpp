#include "deselectabletreeview.h"

DeselectableTreeView::DeselectableTreeView(QWidget * parent)
 : QTreeView(parent)
{

}

DeselectableTreeView::~DeselectableTreeView()
{

}

void DeselectableTreeView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());

    bool selected = selectionModel()->isSelected(indexAt(event->pos()));

    QTreeView::mousePressEvent(event);

    if ((item.row() == -1 && item.column() == -1) || selected)
    {
        clearSelection();
        const QModelIndex index;
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
    }
}
