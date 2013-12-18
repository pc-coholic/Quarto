
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

#include "network.h"
#include "config.h"
#include "logger.h"
#include "prozessSync.h"
#include "performConnection.h"

#ifndef gandalf
#define gandalf

#define WRITE 1
#define READ 0


int pipe_read(int pipe_fd[]);
int pipe_write(int pipe_fd[]);
int ueberwacheFd(int pipe_fd[]);


#endif 