#include "prozessSync.h"
#include "sharedmemory.h"
#include "denkmal.h"

//in performConnection einbauen
// so aufrufen: connector2thinker(getppid());
void connector2thinker( pid_t pid ) {
   kill(pid, SIGUSR1);
}

void signalHandler(int signum) {
	//if (shmPtr->flag == 0) {
	//	return;
	//}
	////Flag setzen
	//shmPtr->flag = 0;
	printf("Signal Handler!!!!\n");
	fflush(stdout);
	think();
}

