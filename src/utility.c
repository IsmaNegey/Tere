#include <stdio.h>
#include <stdlib.h>

#include "utility.h"

void documentRealloc(Document* doc) {
    if (doc->line_count + 1 >= doc->capacity) {
        size_t new_capacity = doc->capacity * 2;

        Line* tmp = realloc(doc->lines, new_capacity * sizeof(Line));
        if (tmp != NULL) {
            doc->lines = tmp;
            for (size_t i = doc->capacity; i < new_capacity; ++i) {
                doc->lines[i].len = 0;
                doc->lines[i].capacity = 16;
                doc->lines[i].line = malloc(doc->lines[i].capacity * sizeof(char));
            }
            doc->capacity = new_capacity;
        }
    }
}

void lineRealloc(Document* doc, size_t length, size_t index) {
    if (length >= doc->lines[index].capacity) {
        size_t new_capacity = doc->lines[index].capacity * 2;
        char* tmp = realloc(doc->lines[index].line, new_capacity);
        if (tmp != NULL) {
            doc->lines[index].line = tmp;
            doc->lines[index].capacity = new_capacity;
            printf("CAPACITY * 2 %ld", doc->lines[index].capacity);
        }
    }
}