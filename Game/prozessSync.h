#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "logger.h"

#ifndef prozesssync
#define prozesssync

#define MAX 5

void connector2thinker(pid_t pid);
void signalHandler();
void endHandler();
#endif
