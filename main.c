#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <termios.h>

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



void print(Document doc, Cursor cursor, char c) {
    printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
    printf("\x1b[%ld;2H", doc.line_count + 2);
    printf("%ld  %ld     %c", cursor.y, cursor.x, c);
    printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
    fflush(stdout);
}


void docRealloc(Document* doc) {

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

    if (doc->lines[doc->line_count].len >= doc->lines[doc->line_count].capacity) {
        size_t new_capacity = doc->lines[doc->line_count].capacity * 2;
        
        char* tmp = realloc(doc->lines[doc->line_count].line, new_capacity);
        if (tmp != NULL) {
            doc->lines[doc->line_count].line = tmp;
            doc->lines[doc->line_count].capacity = new_capacity;
            printf("CAPACITY * 2 %ld", doc->lines[doc->line_count].capacity);
        }

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

    int file = open(file_name, O_RDONLY);

    Document doc = {0};
    char buffer[4096];
    
    doc.capacity = 8;

    doc.lines = malloc(doc.capacity * sizeof(Line));
    for (size_t i = 0; i < doc.capacity; ++i) {
        doc.lines[i].len = 0;
        doc.lines[i].capacity = 16;
        doc.lines[i].line = malloc(doc.lines[i].capacity * sizeof(char));
    }

    size_t buff_size = 0;
    while((buff_size = read(file, buffer, sizeof(buffer))) > 0) {
        for (size_t j = 0; j < buff_size; ++j) {

            if (doc.line_count + 1 >= doc.capacity) {
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

            if (doc.lines[doc.line_count].len >= doc.lines[doc.line_count].capacity) {
                size_t new_capacity = doc.lines[doc.line_count].capacity * 2;
                
                char* tmp = realloc(doc.lines[doc.line_count].line, new_capacity);
                if (tmp != NULL) {
                    doc.lines[doc.line_count].line = tmp;
                    doc.lines[doc.line_count].capacity = new_capacity;
                }
            }

            //docRealloc(&doc);

            doc.lines[doc.line_count].line[doc.lines[doc.line_count].len] = buffer[j];

            if (buffer[j] == '\n') {
                doc.lines[doc.line_count].line[doc.lines[doc.line_count].len] = '\0';
                doc.line_count++;
            }
            else
                doc.lines[doc.line_count].len++;

            
        }
    }

    doc.line_count++;
    for (size_t i = 0; i < doc.line_count; ++i) {
        doc.lines[i].line[doc.lines[i].len] = '\0';
    }

    printf("%zu", doc.line_count);
    
    //doc.lines[doc.line_count - 1].line[doc.lines[doc.line_count - 1].len] = '\0';

    close(file);

    Cursor cursor = {0,0};
    char c;

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


    while (c != 17) {
        size_t n = read(STDIN_FILENO, &c, 1);

        if (n == 1) {



            if (c == 27) {
                n = read(STDIN_FILENO, &c, 1);
                if (c == '[') {
                    n = read(STDIN_FILENO, &c, 1);
                    if (c == 'A' && cursor.y > 0) {
                        if (doc.lines[cursor.y - 1].len == 0 || doc.lines[cursor.y - 1].len - 1 < cursor.x )
                            cursor.x = doc.lines[cursor.y - 1].len;
                        cursor.y--;
                    }
                    else if (c == 'B' && cursor.y < doc.line_count - 1) {
                        if (doc.lines[cursor.y + 1].len == 0 || doc.lines[cursor.y + 1].len - 1 < cursor.x)
                            cursor.x = doc.lines[cursor.y + 1].len;
                        cursor.y++;
                    }
                    else if (c == 'C' && cursor.x < doc.lines[cursor.y].len) {
                        cursor.x++;
                    }
                    else if (c == 'D' && cursor.x > 0) {
                        cursor.x--;
                    }
                    
                }
            }

            else if (c >= 32 && c <= 126) {
                //сдвиг на 1
                if (doc.lines[cursor.y].len + 1 >= doc.lines[cursor.y].capacity) {
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
                for (size_t i = doc.lines[cursor.y].len + 1; i > cursor.x ; --i) {
                        doc.lines[cursor.y].line[i] = doc.lines[cursor.y].line[i - 1];
                }
                doc.lines[cursor.y].line[cursor.x] = c;
                doc.lines[cursor.y].len++;
                cursor.x++;

                doc.lines[cursor.y].line[doc.lines[cursor.y].len] = '\0';

                printf("\x1b[%ld;1H",cursor.y + 1);
                printf("\x1b[K");
                printf("%s", doc.lines[cursor.y].line);

                printf("\x1b[%ld;2H", doc.line_count + 7);
                printf("%ld cap %ld len", doc.lines[cursor.y].capacity, doc.lines[cursor.y].len);
                printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
                
            }
            else if (c == 13) {//Enter
                
                if (doc.line_count + 1 > doc.capacity) {
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

                size_t cur = cursor.y + 1;
                size_t bytes_to_copy = strlen(doc.lines[cursor.y].line + cursor.x); //правая часть строки без \0

                for (size_t i = doc.line_count; i > cur; --i) {
                    doc.lines[i] = doc.lines[i - 1];
                }
                    
                doc.lines[cur].capacity = 16;
                doc.lines[cur].len = 0;
                doc.lines[cur].line = malloc(doc.lines[cur].capacity * sizeof(char));


                memcpy(doc.lines[cur].line, doc.lines[cursor.y].line + cursor.x, bytes_to_copy);
                doc.lines[cur].len = bytes_to_copy;
                doc.lines[cur].line[bytes_to_copy] = '\0';

                doc.lines[cursor.y].line[cursor.x] = '\0';
                doc.lines[cursor.y].len = cursor.x;
                doc.line_count++;

                for (size_t i = 0; i < doc.line_count; ++i) {
                    printf("\x1b[%zu;1H", i + 1);
                    printf("\x1b[K");
                    printf("%s", doc.lines[i].line);
                }
                cursor.y++;
                cursor.x = 0;
            }

            else if (c == 127) {//Delete
                if (cursor.x != 0) {
                    for (size_t i = cursor.x; i < doc.lines[cursor.y].len; ++i) {
                        doc.lines[cursor.y].line[i - 1] = doc.lines[cursor.y].line[i];
                    }
                    doc.lines[cursor.y].len--;
                    doc.lines[cursor.y].line[doc.lines[cursor.y].len] = '\0';
                    cursor.x--;

                }
                else {
                    if (cursor.y != 0) {
                        if (doc.lines[cursor.y - 1].len + doc.lines[cursor.y].len >= doc.lines[cursor.y - 1].capacity) {
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

                        size_t s = doc.lines[cursor.y - 1].len;
                        for (size_t i = 0; i <= doc.lines[cursor.y].len; ++i) {
                            doc.lines[cursor.y - 1].line[s + i] = doc.lines[cursor.y].line[i];
                        }
                        cursor.x = doc.lines[cursor.y - 1].len;
                        doc.lines[cursor.y - 1].len += doc.lines[cursor.y].len;

                        for(size_t i = cursor.y + 1; i < doc.line_count; ++i) {
                            doc.lines[i - 1] = doc.lines[i];
                        }
                        doc.lines[doc.line_count - 1].line = NULL;
                        doc.lines[doc.line_count - 1].len = 0;
                        doc.lines[doc.line_count - 1].capacity = 0;
                        doc.line_count--;
                        
                        
                        
                        cursor.y--;
                        
                    }
                }

                for (size_t i = 0; i < doc.line_count; ++i) {
                    printf("\x1b[%zu;1H", i + 1);
                    printf("\x1b[K");
                    printf("%s", doc.lines[i].line);
                }
                printf("\x1b[%zu;1H", doc.line_count + 1);
                    printf("\x1b[K");
            }

            else if (c == 25) { //Y
                 printf("\x1b[%ld;15H", doc.line_count + 4);
                printf("%c", doc.lines[cursor.y].line[cursor.x]);
                printf("\x1b[%ld;%ldH", cursor.y + 1, cursor.x + 1);
            }

            else if (c == 2) {
                printf("\x1b[K");
                
            }

            else if (c == 15) {

                int file = open("test.txt", O_WRONLY | O_TRUNC);

                for (size_t i = 0; i < doc.line_count; ++i) {
                    char p = '\n';
                    write(file, doc.lines[i].line, doc.lines[i].len);
                    if (i + 1 != doc.line_count)
                        write(file, &p, 1);
                }

                close(file);
            }
        }   

        print(doc, cursor, c);
    }


    printf("\x1b[?1049l"); // вернуться к обычному экрану
    fflush(stdout);

    for (size_t i = 0; i < doc.line_count; ++i) {
        //printf("%s\n", doc.lines[i].line);
        //printf("\x1b[%zu;1H", i + 10);
    }


    
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