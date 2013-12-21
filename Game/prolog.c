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
#include "performConnection.h"
#include "pipe.h"
#include "spielfeldAusgabe.h"

struct shmInfos *shmPtr = NULL; //SHM-Pointer
int *shmPtr_Sf = NULL;

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
	log_printf(LOG_DEBUG,"Using config-file %s\n",confDateiName);
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

	//Shared-Memory erstellen 
	// shmSegment() um die ID zu erstellen -> vor fork()
	int shmid = shmSegment(sizeof(struct shmInfos));

	//und shmAnbinden(shmid); um es an den Prozess zu binden
	shmPtr = shmAnbinden(shmid);

	//shm automatisch entfernen, wenn alle prozesse detached sind
	shmDelete(shmid);
	
	// Pipe anlegen
	int pipe_fd[2];
	if(pipe(pipe_fd)<0){
	
		log_printf(LOG_ERROR,"Fehler bei Pipe anlegen");
	};

	// zweiten Prozess erstellen.
	// Connector ist der Kindprozess
	// Thinker der Elternprozess
	switch (pid = fork ()) {
	case -1:
		log_printf (LOG_ERROR,"Fehler bei fork()\n");
		break;
	case 0: // Connector
		shmPtr->pid1=pid;
			

		//Verbindung mit Server herstellen
		netConnect(configstruct.port, configstruct.hostname);

		while (1 == 1) {
			char *getText;
			char testText[6];

			getText = netReadLine();
			//printf("%s\n", getText);
			strncpy(testText, getText, 6);
			if (strcmp(testText, "+ MNM ") == 0) {
				//+ MNM Gameserver v1.0 accepting connections
				sendVersion();
			} else if (strcmp(testText, "+ Clie") == 0) {
				//+ Client version accepted - please send Game-ID to join
				sendGameId(gameId);
			} else if (strcmp(testText, "+ PLAY") == 0) {
				//+ PLAYING Quarto
				parseGamekind(getText, shmPtr);
				sendPlayer(player, shmPtr);
			} else if (strcmp(testText, "+ ENDP") == 0) {
				//+ ENDPLAYERS
				//noop
			} else if (strcmp(testText, "+ MOVE") == 0) {
				//+ MOVE 3000
				parseMovetimeout(getText, shmPtr);
			} else if (strcmp(testText, "+ NEXT") == 0) {
				//+ NEXT 7
				parseNext(getText, shmPtr);
			} else if (strcmp(testText, "+ FIEL") == 0) {
				//+ FIELD 4,4
				parseField(getText);
			} else if (strcmp(testText, "+ ENDF") == 0) {
				//+ ENDFIELD
				sendThinking();
				//Hier Zug berechnen und per sendMove(stein, naechsterstein) senden
				if(ueberwacheFd(pipe_fd)==1){
					sendMove();
					log_printf(LOG_DEBUG,"Gandalf (PIPE) hat gesprochen und wurde vor dem ertrinken gerettet!\n");
				}
				else{
					log_printf(LOG_PRINTF,"Gandalf ist ersoffen\n");
				}
			}else if (strcmp(testText, "+ WAIT") == 0) {
				//+ WAIT
				sendOkwait();
				
				
			} else if (strcmp(testText, "+ GAME") == 0) {
				//+ GAMEOVER [[ hh Spielernummer des Gewinners ii hh Spielername des Gewinners ii ]]
				parseGameover(getText);
				parseField(getText);
			} else if (strcmp(testText, "+ QUIT") == 0) {
				//+ QUIT
				//netDisconnect();
				break;
			} else if (strcmp(testText, "- Sock") == 0) {
				//- Socket timeout - please be quicker next time
				//Well, fuck.
				//netDiscconnect();
				break;
			}
		}

		break;

	default: // Thinker
		
		shmPtr->pid0=pid;

		signal(SIGUSR1, signalHandler);
		// Auf Signal warten	
		pause();
			
		// In die Pipe schreiben
			pipe_write(pipe_fd);

			
		if (wait (NULL) != pid) {
			log_printf(LOG_ERROR,"Fehler beim Warten auf den Kindprozess\n");
			return EXIT_FAILURE;
		}
		break;
	}

	free(confDateiName);
	return 0;
}
