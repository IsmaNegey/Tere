#ifndef UTILITY_H
#define UTILITY_H

typedef struct {
    char* line;
    size_t len;
    size_t capacity;
} Line;

typedef struct {
    Line* lines;
    size_t line_count;
    size_t capacity;
} Document;

typedef struct {
    size_t x;
    size_t y;
} Cursor;

void documentRealloc(Document* doc);

void lineRealloc(Document* doc, size_t length, size_t index);

#endif