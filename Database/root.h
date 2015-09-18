#ifndef ROOT_H
#define ROOT_H

#include "general.h"
#include "group.h"

class Root : public QObject, public Group
{
    Q_OBJECT

    public:

        explicit Root(QString const & name = QString(""));

        virtual void setModified() override;


    signals:

        void modified();

};

#endif // ROOT_H
