#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <termios.h>
#include "edit.h"
#include "file.h"
#include "utility.h"

void print(Document doc, Cursor cursor, char c) {
    printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
    printf("\x1b[%ld;2H", doc.line_count + 2);
    printf("%ld  %ld     %c", cursor.y, cursor.x, c);
    printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
    fflush(stdout);
}

void mainCycle(Document* doc, char* file_name) {
    Cursor cursor = {0,0};
    char symb;
    while (symb != 17) {
        size_t n = read(STDIN_FILENO, &symb, 1);

        if (n == 1) {

            if (symb == 27) {
                n = read(STDIN_FILENO, &symb, 1);
                if (symb == '[') {
                    n = read(STDIN_FILENO, &symb, 1);
                    if (symb == 'A' && cursor.y > 0) {
                        if (doc->lines[cursor.y - 1].len == 0 || doc->lines[cursor.y - 1].len - 1 < cursor.x )
                            cursor.x = doc->lines[cursor.y - 1].len;
                        cursor.y--;
                    }
                    else if (symb == 'B' && cursor.y < doc->line_count - 1) {
                        if (doc->lines[cursor.y + 1].len == 0 || doc->lines[cursor.y + 1].len - 1 < cursor.x)
                            cursor.x = doc->lines[cursor.y + 1].len;
                        cursor.y++;
                    }
                    else if (symb == 'C' && cursor.x < doc->lines[cursor.y].len) {
                        cursor.x++;
                    }
                    else if (symb == 'D' && cursor.x > 0) {
                        cursor.x--;
                    } 
                }
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
        print(*doc, cursor, symb);
    }
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        return 1;
    }
    char* file_name = argv[1];

    struct termios raw;
    struct termios original;

    if (tcgetattr(STDIN_FILENO, &original) == -1) {
        perror("tcgetattr");
        return 1;
    }

    raw = original;
    cfmakeraw(&raw);

    // Применяем новые настройки
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        return 1;
    }

    Document doc;

    readFile(file_name, &doc); //чтение файла и запись в массив

    printf("%zu", doc.line_count);
    printf("\x1b[?1049h"); // перейти на альтернативный экран
    printf("\x1b[2J");    // очистить его
    printf("\x1b[H");
    fflush(stdout);

    for (size_t i = 0; i < doc.line_count; ++i) {
        if (doc.lines[i].line == NULL)
            break;
        printf("\x1b[%zu;1H", i + 1);
        printf("%s", doc.lines[i].line);
        fflush(stdout);
    }

    printf("\x1b[1;1H");
    fflush(stdout);

    mainCycle(&doc, file_name); //главный цикл

    printf("\x1b[?1049l"); // вернуться к обычному экрану
    fflush(stdout);

    for (size_t i = 0; i < doc.capacity; ++i) {
        free(doc.lines[i].line);
    }
    free(doc.lines);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1) {
        perror("tcsetattr");
        return 1;
    }

    return 0;
}