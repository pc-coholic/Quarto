#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "sharedmemory.h"
#include "logger.h"

#ifndef spielfeld
#define spielfeld

void printField(int *shmPtr_Sf);
void printSpielstein(int *shmPtr_Sf,int i);

#endif

