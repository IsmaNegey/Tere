#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "file.h"
#include "utility.h"

void readFile(char* file_name, Document* doc) {

    int file = open(file_name, O_RDONLY);

    doc->capacity = 8;
    doc->lines = malloc(doc->capacity * sizeof(Line));

    for (size_t i = 0; i < doc->capacity; ++i) {
        doc->lines[i].len = 0;
        doc->lines[i].capacity = 16;
        doc->lines[i].line = malloc(doc->lines[i].capacity * sizeof(char));
    }

    char buffer[4096];
    size_t buff_size = 0;
    while((buff_size = read(file, buffer, sizeof(buffer))) > 0) {
        for (size_t j = 0; j < buff_size; ++j) {
            documentRealloc(doc);

            /*if (doc.line_count + 1 >= doc.capacity) {
                size_t new_capacity = doc.capacity * 2;

                Line* tmp = realloc(doc.lines, new_capacity * sizeof(Line));
                if (tmp != NULL) {
                    doc.lines = tmp;
                    for (size_t i = doc.capacity; i < new_capacity; ++i) {
                        doc.lines[i].len = 0;
                        doc.lines[i].capacity = 16;
                        doc.lines[i].line = malloc(doc.lines[i].capacity * sizeof(char));
                    }
                    doc.capacity = new_capacity;
                }
            }
            */

            lineRealloc(doc, doc->lines[doc->line_count].len, doc->line_count);

            /*if (doc.lines[doc.line_count].len >= doc.lines[doc.line_count].capacity) {
                size_t new_capacity = doc.lines[doc.line_count].capacity * 2;
                
                char* tmp = realloc(doc.lines[doc.line_count].line, new_capacity);
                if (tmp != NULL) {
                    doc.lines[doc.line_count].line = tmp;
                    doc.lines[doc.line_count].capacity = new_capacity;
                    printf("CAPACITY * 2 %ld", doc.lines[doc.line_count].capacity);
                }

            }*/

            //docRealloc(&doc);

            doc->lines[doc->line_count].line[doc->lines[doc->line_count].len] = buffer[j];

            if (buffer[j] == '\n') {
                doc->lines[doc->line_count].line[doc->lines[doc->line_count].len] = '\0';
                doc->line_count++;
            }
            else
                doc->lines[doc->line_count].len++;
        }
    }

    doc->line_count++;
    for (size_t i = 0; i < doc->line_count; ++i) {
        doc->lines[i].line[doc->lines[i].len] = '\0';
    }

    close(file);
}


void writeFile(char* file_name, Document* doc) {

    int file = open(file_name, O_WRONLY | O_TRUNC);

    for (size_t i = 0; i < doc->line_count; ++i) {
        char p = '\n';
        write(file, doc->lines[i].line, doc->lines[i].len);
        if (i + 1 != doc->line_count)
            write(file, &p, 1);
    }
    close(file);
}

