#include "descriptioneditor.h"

DescriptionEditor::DescriptionEditor(QWidget *parent)
 : QPlainTextEdit(parent)
{
    //Nothing to do here
}

void DescriptionEditor::focusOutEvent(QFocusEvent * e)
{
    emit newContent(this->toPlainText());
}

//Yeah, I know, it's quite painful to have to create a whole class just for that...
