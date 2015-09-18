#ifndef DESCRIPTIONEDITOR_H
#define DESCRIPTIONEDITOR_H

#include "general.h"

class DescriptionEditor : public QPlainTextEdit
{
    Q_OBJECT

    public:

        explicit DescriptionEditor(QWidget *parent = NULL);


    private:

        virtual void focusOutEvent(QFocusEvent * e) override;


    signals:

        void newContent(QString const & text);

};

#endif // DESCRIPTIONEDITOR_H
