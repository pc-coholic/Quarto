#include "logger.h"

void log_printf(int l, int level, const char *msg)
{
    if (level > l) return;

    printf("%s", msg);
}

void log_info(int l, const char *fmt, ...)
{
    va_list ap;
    char msg[LOG_MAX_MSG_LEN];

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    log_printf(l, LOG_INFO, msg);
    va_end(ap);
}

void log_more_info(int l, const char *fmt, ...)
{
    va_list ap;
    char msg[LOG_MAX_MSG_LEN];
    
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    log_printf(l, LOG_MORE_INFO, msg);
    va_end(ap);
}

void log_error(int l, const char *fmt, ...)
{
    va_list ap;
    char msg[LOG_MAX_MSG_LEN];
    
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    log_printf(l, LOG_ERROR, msg);
    va_end(ap);
}
