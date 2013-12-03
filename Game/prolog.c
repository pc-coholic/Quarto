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

#define GAMEKINDNAME "Quarto"
#define PORT "1357"
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
	
int	performConnection(char *gameId, char player, struct shmInfos *shmPtr);

// Nutzungsbeschreibung des Clienten
void help() {
	printf("\nNutzung des Clients:	client [Game-ID] -p [0|1] -c [dateinname]\n");
	printf("\n	-p:	gewünschte Spielernummer. 0 für Spieler 1, 1 für Spieler 2\n");
	printf("	-c:	andere Konfigurationsdatei (Standard: client.conf)\n\n");
}

int main(int argc, char *argv[]) {
	int ret; //fuer getopt funktion
	char player = '3';
	char gameId[15];
	char confDateiName[256] = "client.conf";
	printf("%s",confDateiName);
	//FILE* configDatei = NULL; 

	pid_t pid;
	
	//11-stellige Game-Id aus Kommandozeile auslesen
	if (argc < 2) {
		printf("Keine Game-Id angegeben!");
		help();
		exit(EXIT_FAILURE);
	}
	strcpy(gameId,argv[1]);
	
	// Konfigurationsdatei implementieren fuer Meilenstein 2/1.
	/*
	strcpy(dateiName,argv[2]);
	configDatei = openFile(dateiName);
	*/
	
	if(strlen(gameId) != 11) {
		printf("Game-Id muss 11-stellig sein!");
		help();
		exit(EXIT_FAILURE);
	}

	//optional gewunschte Spielernummer und config Dateiname einlesen 
	while ((ret=getopt(argc, argv, "p:c:")) != -1) {
	switch (ret) {
	case 'p':
		player = optarg[0];
		if (player!='0' && player != '1') {
			printf("Es gibt nur 2 Spieler! 0 oder 1 eingeben!");
			help();
			exit(EXIT_FAILURE);
		}
		break;
	case 'c':
		confDateiName[0] = *optarg;
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
	switch (pid = fork ()) {
	case -1:
		printf ("Fehler bei fork()");
		break;
	case 0: // Connector
		
		//Verbindung mit Server herstellen
		netConnect(PORT, HOSTNAME);

		performConnection(gameId, player, shmPtr);
	
		//Verbindung zum Server trennen
		//netDisconnect();
		break;

	default: // Thinker
		if (wait (NULL) != pid) {
			printf("Fehler beim Warten auf den Kindprozess");
			return EXIT_FAILURE;
		}
		break;
	}
     
	return 0;
}
