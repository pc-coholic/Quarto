#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "logger.h"

#ifndef prozesssync
#define prozesssync

#define MAX 5
//Connector schickt Thinker, dass er seines Amtes walten soll (denken)
void connector2thinker(pid_t pid);
//Flag prüfen, setzen und think einsetzen
void signalHandler();
//Child->Parent: Eltern-Prozess soll sich beenden (bei - vom Server)
void endHandler();
#endif
