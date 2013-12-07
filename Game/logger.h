#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "config.h"

#ifndef logger
#define logger

#define LOG_ERROR 0
#define LOG_PRINTF 1
#define LOG_DEBUG 2

#define LOG_MAX_MSG_LEN 1024

// Funktion zum printen: Bei Level eine der oben genannten #defines angeben
void log_printf(int level, const char *msg, ...);

#endif
