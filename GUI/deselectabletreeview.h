#ifndef DESELECTABLETREEVIEW_H
#define DESELECTABLETREEVIEW_H

#include "general.h"

class DeselectableTreeView : public QTreeView
{
    public:

        DeselectableTreeView(QWidget * parent);
        ~DeselectableTreeView();


    private:

        virtual void mousePressEvent(QMouseEvent *event) override;

};

#endif // DESELECTABLETREEVIEW_H
