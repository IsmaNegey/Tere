#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "edit.h"
#include "file.h"
#include "utility.h"
#include "print.h"

void mainCycle(Document* doc, char* file_name) {
    Cursor cursor = {0,0};
    char symb;
    while (symb != 17) {
        size_t n = read(STDIN_FILENO, &symb, 1);

        if (n == 1) {
            if (symb == 27) {
                manageCursor(*doc, &cursor);
            }
            else if (symb >= 32 && symb <= 126) {
                addSymbol(doc, &cursor, symb);
            }
            else if (symb == 13) {
                enterButton(doc, &cursor); 
            }
            else if (symb == 127) {
                deleteButton(doc, &cursor);
            }
            else if (symb == 2) {//B
                printf("\x1b[K");
            }
            else if (symb == 15) {
                writeFile(file_name, doc);
            }
        }   
        printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
        fflush(stdout);
    }
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        return 1;
    }
    char* file_name = argv[1];

    struct termios original;

    enableRawMode(&original); //переход на новый экран

    Document doc = {0};

    readFile(file_name, &doc); //чтение файла и запись в массив

    printFile(doc); //начальный вывод всего файла

    printf("\x1b[1;1H");
    fflush(stdout);

    mainCycle(&doc, file_name); //главный цикл

    printf("\x1b[?1049l"); // вернуться к обычному экрану
    fflush(stdout);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1) {
        perror("tcsetattr");
        return 1;
    }

    for (size_t i = 0; i < doc.capacity; ++i) {
        free(doc.lines[i].line);
    }
    free(doc.lines);
    
    return 0;
}