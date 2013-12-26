#include "logger.h"

void log_printf(int level, const char *fmt, ...)
{
    if (level > configstruct.loglevel) return;

    va_list ap;
    char msg[LOG_MAX_MSG_LEN];

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    //log(l, LOG_INFO, msg);
    va_end(ap);

    if (level == LOG_ERROR) {
	printf("\033[1m\033[31m"); //rot und bold
    }
    printf("%s""\033[0m", msg); // msg ausgabe und umstellung auf normale Ausgabe
}
