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

#include "network.h"
#include "sharedmemory.h"

//prozess
#include <sys/wait.h>
#include <signal.h>

#define GAMEKINDNAME "Quarto"
#define PORT "1357"
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
	
int	performConnection(char *gameId, char player);

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
		perror("Keine Game-Id angegeben!");
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
		perror("Game-Id muss 11-stellig sein!");
		help();
		exit(EXIT_FAILURE);
	}

	//optional gewunschte Spielernummer und config Dateiname einlesen 
	while ((ret=getopt(argc, argv, "p:c:")) != -1) {
	switch (ret) {
	case 'p':
		player = optarg[0];
		if (player!='0' && player != '1') {
			perror("Es gibt nur 2 Spieler! 0 oder 1 eingeben!");
			help();
			exit(EXIT_FAILURE);
		}
		break;
	case 'c':
		confDateiName[256] = *optarg;
		break;
	default:
		help();
		exit(EXIT_FAILURE);
	}
	}


	// zweiten Prozess erstellen.
	// Connector ist der Kindprozess
	// Thinker der Elternprozess
	switch (pid = fork ()) {
	case -1:
		perror ("Fehler bei fork()");
		break;
	case 0: // Connector
	
		//Ich will hier das SharedMemory Segment ueber meine Funktionen erstellen
		// shmSegment() um die ID zu erstellen und shmAnbinden(shmid); um es an den Prozess zu binden. 
		// Was mach ich fuer Logikfehler, dass die Funktionen hier nicht funktionieren?
		int shmid = shmSegment();
		shmAnbinden(shmid);
		
		//Verbindung mit Server herstellen
		netConnect(PORT, HOSTNAME);

		performConnection(gameId, player);
	
		//Verbindung zum Server trennen
		//netDisconnect();
		break;

	default: // Thinker
		if (wait (NULL) != pid) {
			perror("Fehler beim Warten auf den Kindprozess");
			return EXIT_FAILURE;
		}
		break;
	}
     
	return 0;
}
