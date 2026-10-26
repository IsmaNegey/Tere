#ifndef PRINT_H
#define PRINT_H

#include "utility.h"
#include <stddef.h>
#include <termios.h>

void printFile(Document doc);

int enableRawMode(struct termios *original);

#endif