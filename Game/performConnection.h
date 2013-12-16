#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "network.h"
#include "logger.h"
#include "sharedmemory.h"
#include "denkmal.h"
#include "prozessSync.h"

#define CLIENTVERSION "1.42"
#define BUF 251

#ifndef	prfrmCnnctn
#define prfrmCnnctn
	int performConnection( char* gameId, int player, struct shmInfos *shmPtr);
	void checkMinus();
	void sendVersion();
	void sendGameId(char* gameId);
	void parseGamekind(char* getText, struct shmInfos *shmPtr);
	void sendPlayer(int player, struct shmInfos *shmPtr);
	void parseMovetimeout(char* getText, struct shmInfos *shmPtr);
	void parseNext(char* getText, struct shmInfos *shmPtr);
	void parseField(char* getText, struct shmInfos *shmPtr);
	void sendThinking();
	void parseGameover(char* getText);
#endif
