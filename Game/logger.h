#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#ifndef _LOGGER_H_
#define _LOGGER_H_

#define LOG_ERROR 0
#define LOG_INFO 1
#define LOG_MORE_INFO 2

#define LOG_MAX_MSG_LEN 1024

void log_printf(int log, int level, const char *msg);
void log_error(int log, const char *fmt, ...);
void log_info(int log, const char *fmt, ...);
void log_more_info(int log, const char *fmt, ...);

#endif
