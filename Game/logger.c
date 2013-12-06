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

    printf("%s", msg);
}
