#include "print.h"

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

void printFile(Document doc) {
    for (size_t i = 0; i < doc.line_count; ++i) {
        if (doc.lines[i].line == NULL)
            break;
        printf("\x1b[%zu;1H", i + 1);
        printf("%s", doc.lines[i].line);
        fflush(stdout);
    }
}

int enableRawMode(struct termios *original) {
   
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, original) == -1) {
        perror("tcgetattr");
        return 1;
    }

    raw = *original;
    cfmakeraw(&raw);

    // Применяем новые настройки
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("\x1b[?1049h"); // перейти на альтернативный экран
    printf("\x1b[2J");    // очистить его
    printf("\x1b[H");
    fflush(stdout);

    return 0;
}