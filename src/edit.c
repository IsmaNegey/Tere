#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include "edit.h"
#include "utility.h"

void addSymbol(Document* doc, Cursor* cursor, char symb) {
    lineRealloc(doc, doc->lines[cursor->y].len + 1, cursor->y);

    /*if (doc.lines[cursor.y].len + 1 >= doc.lines[cursor.y].capacity) {
        size_t new_capacity = doc.lines[cursor.y].capacity * 2;

        char* tmp = realloc(doc.lines[cursor.y].line, new_capacity);
        if (tmp != NULL) {
            doc.lines[cursor.y].line = tmp;
            doc.lines[cursor.y].capacity = new_capacity;
        }
        printf("\x1b[%ld;2H", doc.line_count + 5);
        printf("Capacity %ld is increased: from %ld to %ld", cursor.y + 1, doc.lines[cursor.y].capacity / 2, doc.lines[cursor.y].capacity);
        printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);

    }
    */
    for (size_t i = doc->lines[cursor->y].len + 1; i > cursor->x ; --i) {
            doc->lines[cursor->y].line[i] = doc->lines[cursor->y].line[i - 1];
    }
    doc->lines[cursor->y].line[cursor->x] = symb;
    doc->lines[cursor->y].len++;
    cursor->x++;

    doc->lines[cursor->y].line[doc->lines[cursor->y].len] = '\0';

    printf("\x1b[%ld;1H",cursor->y + 1);
    printf("\x1b[K");
    printf("%s", doc->lines[cursor->y].line);

    printf("\x1b[%ld;2H", doc->line_count + 7);
    printf("%ld cap %ld len", doc->lines[cursor->y].capacity, doc->lines[cursor->y].len);
    printf("\x1b[%ld;%ldH", cursor->y + 1, cursor->x + 1);
}

void enterButton(Document* doc, Cursor* cursor) {
    documentRealloc(doc);

    size_t cur = cursor->y + 1;
    size_t bytes_to_copy = strlen(doc->lines[cursor->y].line + cursor->x); //правая часть строки без \0

    for (size_t i = doc->line_count; i > cur; --i) {
        doc->lines[i] = doc->lines[i - 1];
    }
        
    doc->lines[cur].capacity = 16;
    doc->lines[cur].len = 0;
    doc->lines[cur].line = malloc(doc->lines[cur].capacity * sizeof(char));

    memcpy(doc->lines[cur].line, doc->lines[cursor->y].line + cursor->x, bytes_to_copy);
    doc->lines[cur].len = bytes_to_copy;
    doc->lines[cur].line[bytes_to_copy] = '\0';

    doc->lines[cursor->y].line[cursor->x] = '\0';
    doc->lines[cursor->y].len = cursor->x;
    doc->line_count++;

    for (size_t i = 0; i < doc->line_count; ++i) {
        printf("\x1b[%zu;1H", i + 1);
        printf("\x1b[K");
        printf("%s", doc->lines[i].line);
    }
    cursor->y++;
    cursor->x = 0;
}

void deleteButton(Document* doc, Cursor* cursor) {
    if (cursor->x != 0) {
        for (size_t i = cursor->x; i < doc->lines[cursor->y].len; ++i) {
            doc->lines[cursor->y].line[i - 1] = doc->lines[cursor->y].line[i];
        }
        doc->lines[cursor->y].len--;
        doc->lines[cursor->y].line[doc->lines[cursor->y].len] = '\0';
        cursor->x--;

    }
    else if (cursor->y != 0) {

        lineRealloc(doc, doc->lines[cursor->y - 1].len + doc->lines[cursor->y].len, cursor->y);
        /*if (doc.lines[cursor.y - 1].len + doc.lines[cursor.y].len >= doc.lines[cursor.y - 1].capacity) {
            size_t new_capacity = doc.lines[cursor.y].capacity * 2;

            char* tmp = realloc(doc.lines[cursor.y].line, new_capacity);
            if (tmp != NULL) {
                doc.lines[cursor.y].line = tmp;
                doc.lines[cursor.y].capacity = new_capacity;
            }
            printf("\x1b[%ld;2H", doc.line_count + 5);
            printf("Capacity %ld is increased: from %ld to %ld", cursor.y + 1, doc.lines[cursor.y].capacity / 2, doc.lines[cursor.y].capacity);
            printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
        }
        */

        size_t s = doc->lines[cursor->y - 1].len;
        for (size_t i = 0; i <= doc->lines[cursor->y].len; ++i) {
            doc->lines[cursor->y - 1].line[s + i] = doc->lines[cursor->y].line[i];
        }
        cursor->x = doc->lines[cursor->y - 1].len;
        doc->lines[cursor->y - 1].len += doc->lines[cursor->y].len;

        for(size_t i = cursor->y + 1; i < doc->line_count; ++i) {
            doc->lines[i - 1] = doc->lines[i];
        }
        doc->lines[doc->line_count - 1].line = NULL;
        doc->lines[doc->line_count - 1].len = 0;
        doc->lines[doc->line_count - 1].capacity = 0;
        doc->line_count--;
        
        cursor->y--;
    }
    
    for (size_t i = 0; i < doc->line_count; ++i) {
        printf("\x1b[%zu;1H", i + 1);
        printf("\x1b[K");
        printf("%s", doc->lines[i].line);
    }
    printf("\x1b[%zu;1H", doc->line_count + 1);
        printf("\x1b[K");
}

void manageCursor(Document doc, Cursor* cursor) {
    char symb;
    size_t n = read(STDIN_FILENO, &symb, 1);
    if (n == 1) {
        if (symb == '[') {
            n = read(STDIN_FILENO, &symb, 1);
            if (symb == 'A' && cursor->y > 0) {
                if (doc.lines[cursor->y - 1].len == 0 || doc.lines[cursor->y - 1].len - 1 < cursor->x )
                    cursor->x = doc.lines[cursor->y - 1].len;
                cursor->y--;
            }
            else if (symb == 'B' && cursor->y < doc.line_count - 1) {
                if (doc.lines[cursor->y + 1].len == 0 || doc.lines[cursor->y + 1].len - 1 < cursor->x)
                    cursor->x = doc.lines[cursor->y + 1].len;
                cursor->y++;
            }
            else if (symb == 'C' && cursor->x < doc.lines[cursor->y].len) {
                cursor->x++;
            }
            else if (symb == 'D' && cursor->x > 0) {
                cursor->x--;
            }
        } 
    }
}