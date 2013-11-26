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

//prozess
#include <sys/wait.h>
#include <signal.h>

#define GAMEKINDNAME "Quarto"
#define PORT "1357"
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
	
int	performConnection(char *gameId, char player);

int main(int argc, char *argv[]) {
	int ret; //fuer getopt funktion
	char player = '3';
	char gameId[15];
	//FILE* configDatei = NULL; 
	//char dateiName[256];
	pid_t pid;
	int prozessStatus;
	
	//11-stellige Game-Id aus Kommandozeile auslesen
	if (argc < 2) {
		perror("Keine Game-Id angegeben!");
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
		exit(EXIT_FAILURE);
	}

	//optional gewunschte Spielernummer einlesen 
	while ((ret=getopt(argc, argv, "p:")) != -1) {
	switch (ret) {
	case 'p':
		player = optarg[0];
		if (player!='0' && player != '1') {
			perror("Es gibt nur 2 Spieler! 0 oder 1 eingeben!");
			exit(EXIT_FAILURE);
		}
		break;
	}
	}


	// zweiten Prozess erstellen.
	// Connector ist der Kindprozess
	// Thinker der Elternprozess
	switch (pid = fork ()) {
	case -1:
		perror ("Fehler bei fork()\n");
		break;
	case 0: // Connector
		
		//Verbindung mit Server herstellen
		netConnect(PORT, HOSTNAME);

		performConnection(gameId, player);
	
		//Verbindung zum Server trennen
		//netDisconnect();
		break;

	default: // Thinker
		if (wait (&prozessStatus) != pid) {
			perror("Fehler beim Warten auf den Kindprozess");
			return EXIT_FAILURE;
		}
		break;
	}
     
	return 0;
}
