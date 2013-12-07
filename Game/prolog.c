#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
//prozess
#include <sys/wait.h>
#include <signal.h>

#include "sharedmemory.h"
#include "network.h"
#include "config.h"
#include "logger.h"
#include "prozessSync.h"
	
int	performConnection(char *gameId, char player, struct shmInfos *shmPtr);

// Nutzungsbeschreibung des Clienten
void help() {
	printf("\nNutzung des Clienten:	client [Game-ID] -p [0|1] -c [dateinname] -l [0|1|2]\n");
	printf("\n	-p:	gewünschte Spielernummer: 0 für Spieler 1, 1 für Spieler 2\n");
	printf("	-c:	andere Konfigurationsdatei (Default: client.conf)\n");
	printf("	-l:	Loglevel: 0 für Errors, 1 für Spielinfo, 2 für Debuginfo (Default steht in Konfigurationsdatei\n");
	printf("\n");
}

int main(int argc, char *argv[]) {
	int ret; //fuer getopt funktion
	char player = '3';
	char gameId[15];
	char *confDateiName = malloc(256);
	strcpy(confDateiName, "client.conf");
	pid_t pid;

    //Config-Datei einlesen und struct betanken
	log_printf(LOG_PRINTF,"Using config-file %s\n",confDateiName);
	configstruct = get_config(confDateiName);

	log_printf(LOG_PRINTF,"\n");

	//11-stellige Game-Id aus Kommandozeile auslesen
	if (argc < 2) {
		printf("Keine Game-Id angegeben!\n");
		help();
		exit(EXIT_FAILURE);
	}
	strcpy(gameId,argv[1]);
	
	if(strlen(gameId) != 11) {
		printf("Game-Id muss 11-stellig sein!\n");
		help();
		exit(EXIT_FAILURE);
	}

	//optional gewunschte Spielernummer und config Dateiname einlesen 
	while ((ret=getopt(argc, argv, "p:c:l:")) != -1) {
	switch (ret) {
	case 'p':
		player = optarg[0];
		if (player!='0' && player != '1') {
			printf("Es gibt nur 2 Spieler! 0 oder 1 eingeben!\n");
			help();
			exit(EXIT_FAILURE);
		}
		break;
	case 'c':
		strcpy(confDateiName, optarg);
		break;
	case 'l':
		configstruct.loglevel = atoi(optarg);
		break;
	default:
		help();
		exit(EXIT_FAILURE);
	}
	}

	//Shared-Memory anbinden
	// shmSegment() um die ID zu erstellen -> vor fork()
	int shmid = shmSegment();

	//und shmAnbinden(shmid); um es an den Prozess zu binden.-> muss dann in jeden Prozess einzeln
	struct shmInfos *shmPtr;
	shmPtr = shmAnbinden(shmid);
	//shmPtr->eigSpielernummer = 10;
	//printf("%i",shmPtr->eigSpielernummer);

	//shm automatisch entfernen, wenn alle prozesse detached
	shmDelete(shmid);

	// zweiten Prozess erstellen.
	// Connector ist der Kindprozess
	// Thinker der Elternprozess
	
	start_sync();
	switch (pid = fork ()) {
	case -1:
		log_printf (LOG_ERROR,"Fehler bei fork()\n");
		break;
	case 0: // Connector
			shmPtr->pid1=pid;
		//Verbindung mit Server herstellen
		netConnect(configstruct.port, configstruct.hostname);

		performConnection(gameId, player, shmPtr);
	
		//Verbindung zum Server trennen
		//netDisconnect();

		//pidNull();
		break;

	default: // Thinker
		shmPtr->pid0=pid;
		//pidelse(pid);

		if (wait (NULL) != pid) {
			log_printf(LOG_ERROR,"Fehler beim Warten auf den Kindprozess\n");
			return EXIT_FAILURE;
		}
		break;
	}

	free(confDateiName);
	return 0;
}
