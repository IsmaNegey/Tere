#ifndef PRINT_H
#define PRINT_H

#include "utility.h"
#include <stddef.h>
#include <termios.h>

#include <sys/ioctl.h> //для размера терминала

int getWinsize(struct winsize* ws);

void printFile(Document doc, struct winsize *ws, size_t row_cursor, size_t *cur_max);

int enableRawMode(struct termios *original);

#endif