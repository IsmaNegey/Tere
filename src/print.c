#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "edit.h"
#include "file.h"
#include "utility.h"

int getWinsize(struct winsize* ws) {

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, ws) == -1) {
        perror("ioctl");
        return 1;
    }
    return 0;
}


void printFile(Document doc, struct winsize *ws, size_t row_cursor, size_t *cur_max) {
    if (getWinsize(ws) != 0)
        return;

    size_t height = ws->ws_row;

    if (height == 0 || doc.line_count == 0)
        return;

    if (*cur_max > doc.line_count)
        *cur_max = doc.line_count;

    if (*cur_max == 0)
        *cur_max = height < doc.line_count ? height : doc.line_count;

    size_t start = *cur_max > height ? *cur_max - height : 0;

    if (row_cursor >= *cur_max) {
        *cur_max = row_cursor + 1;
    } else if (row_cursor < start) {
        *cur_max = row_cursor + height;
    }

    if (*cur_max > doc.line_count)
        *cur_max = doc.line_count;

    start = *cur_max > height ? *cur_max - height : 0;

    for (size_t i = 0, line = start; i < height; ++i, ++line) {
        printf("\x1b[%zu;1H", i + 1);
        printf("\x1b[K");

        if (line < *cur_max)
            printf("%s", doc.lines[line].line);
    }

    fflush(stdout);
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