#ifndef EDIT_H
#define EDIT_H

#include "utility.h"

void deleteButton(Document* doc, Cursor* cursor);

void enterButton(Document* doc, Cursor* cursor);

void addSymbol(Document* doc, Cursor* cursor, char symb);

void manageCursor(Document doc, Cursor* cursor);

#endif