#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "sharedmemory.h"
#include "logger.h"

#ifndef spielfeld
#define spielfeld

void printField(struct shmSpielfeld *shmPtr_Sf);
void printSpielstein(struct shmSpielfeld *shmPtr_Sf,int i);

#endif

