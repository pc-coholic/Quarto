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

#define GAMEKINDNAME "Quarto"
#define PORT "1357"
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"

int performConnection(int socket, char* gameId, char player);

int main(int argc, char *argv[]) {
	int socke;
	int ret; //fuer getopt funktion
	char player = '3';
	char gameId[15];
	struct addrinfo hints, *servinfo, *p;

	//11-stellige Game-Id aus Kommandozeile auslesen
	if (argc < 2) {
		perror("Keine Game-Id angegeben!");
		exit(EXIT_FAILURE);
	}
	strcpy(gameId,argv[1]);
	if(strlen(gameId) != 11) {
		perror("Game-Id muss 11-stellig sein!");
		exit(EXIT_FAILURE);
	}

	//optional gewunschte Spielernummer
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

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	//Server-Adresse in IP-Adresse umwandeln
	if (getaddrinfo(HOSTNAME, PORT, &hints, &servinfo) != 0) {
		perror("Fehler beim Servernamen.");
		exit(EXIT_FAILURE);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		// Socket anlegen
		if ((socke = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("Socket konnte nicht angelegt werden.");
				continue;
		}
		// Mit Socket verbinden
		if (connect(socke, p->ai_addr, p->ai_addrlen) == -1) {
			close(socke);
			perror("Verbindung mit Socket fehlgeschlagen.");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "Client konnte nicht verbunden worden.\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(servinfo); // all done with this structure
	
	performConnection(socke, gameId, player);

	return 0;
}
