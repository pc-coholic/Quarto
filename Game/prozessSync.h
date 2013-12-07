#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "logger.h"

#ifndef prozessSync
#define prozessSync

#define MAX 5

typedef void (*sighandler_t)(int);
static volatile sig_atomic_t flag = 0;
void start_sync(void);
void connector_waits(void);
void connector2thinker(pid_t pid);
void thinker2connector(pid_t pid);
void thinker_waits(void);
void pidNull();
void pidelse(pid_t pid);
#endif
