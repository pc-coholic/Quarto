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
#define BUF 251

//Hilfsfunktionen
int netRecv();
int netUpdateBuffer();

//Variablen
int socke;

int bufferBenutzt = 0;//bis zu welcher Stelle von buffer eingelesen wurde
int bufferEnde = 0; //Stelle von newline im buffer
//char bufferSend[BUF];
char bufferGet[BUF];


//Funktion: Liest eine Zeile vom Server
char* netReadLine() {
	while(1) {
  //ist eine ganze Zeile im Buffer? -> ausgeben
	for (int i=0; i<bufferBenutzt; i++) {
		if(bufferGet[i]=='\n') {
	  	bufferGet[i] = '\0';
	    bufferEnde = i;
	    printf("S: %s\n",bufferGet);
	    fflush(stdout);
	    return bufferGet;
	  }
	}
	//ansonsten vom Server empfangen
	netRecv();
	}
}

//Hilfs-Funktion empfaengt Text vom Server
int netRecv(){
	int len = recv(socke, (bufferGet + bufferBenutzt), BUF-1,0);
	bufferBenutzt += len;
	return 1;
}

//Hilfs-Funktion: noch nicht ausgegebenen Teil des Buffers nach vorne schieben + bufferBenutzt ändern
int netUpdateBuffer() {
   for(int j=bufferEnde+1, k=0; j<=bufferBenutzt; j++,k++) {
     bufferGet[k] = bufferGet[j];
   }
   bufferBenutzt -= bufferEnde+1;  
   return 1;
 }

// Schreibt den Text aus *bufferSend in das Socket
int netWrite(char *bufferSend) {
	int len = strlen(bufferSend);
	if(write(socke, bufferSend, len*sizeof(char)) < 0) {
		perror("Fehler beim Schreiben in den Socket");
		return 0;
	}
	printf("C: %s\n",bufferSend);
	fflush(stdout);
	return 1;
}

// Verbindet sich mit dem Server 
int netConnect(char* port, char* hostname ) {
  struct addrinfo hints, *servinfo, *p;


	memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
 
  //Server-Adresse in IP-Adresse umwandeln
  if (getaddrinfo(hostname, port, &hints, &servinfo) != 0) {
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
	
	return 1;
}
