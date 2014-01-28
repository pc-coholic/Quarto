#include "prozessSync.h"
#include "sharedmemory.h"
#include "denkmal.h"

extern struct shmInfos *shmPtr;

//Connector schickt Thinker, dass er denken soll
void connector2thinker( pid_t pid ) {
   kill(pid, SIGUSR1);
}

void signalHandler(int signum) {
	if (shmPtr->flag == 0) {
		return;
	}
	//Flag setzen
	shmPtr->flag = 0;
	//Ein wildes Thinker erscheint. Es setzt think() ein. Es ist (nicht) sehr effektiv.
	think();
}

// Signal-Handler, dass der Eltern-Prozess sich beenden soll (bei - vom Server)
void endHandler(int signum) {
	exit(EXIT_FAILURE);
}	
