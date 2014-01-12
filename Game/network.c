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
#include "logger.h"
#define BUF 251

//Hilfsfunktionen
static int netRecv(char* bufAnf, size_t bufRestLen);
static int netUpdateBuffer(char* bufferGet, int bufferBenutzt, int bufferEnde);

//Variablen
int socke; //Socket

//Funktion: Liest eine Zeile vom Server
char* netReadLine() {
	static int bufferBenutzt = 0;//bis zu welcher Stelle von buffer eingelesen wurde
	static int bufferEnde = 0; //Stelle von newline im buffer
	static char bufferGet[BUF];

	// Wenn bereits Text im Buffer steht: noch nicht ausgegebenen Teil des Buffers nach vorne schieben + bufferBenutzt ändern
	if (bufferBenutzt > 0 && bufferEnde > 0) {
   	bufferBenutzt -= netUpdateBuffer(bufferGet, bufferBenutzt, bufferEnde);
	}

	while(1) {
  //ist eine ganze Zeile im Buffer? -> ausgeben
	for (int i=0; i<bufferBenutzt; i++) {
		if(bufferGet[i]=='\n') {
	  	bufferGet[i] = '\0';
	    bufferEnde = i;
	   	log_printf(LOG_DEBUG,"\033[1m\033[33m""S: ""\033[0m""%s\n",bufferGet); // bold yellow
	   // fflush(stdout);
	    return bufferGet;
	  }
	}
	//ansonsten vom Server empfangen
	bufferBenutzt += netRecv(bufferGet + bufferBenutzt, BUF-bufferBenutzt);
	}
}

//Hilfs-Funktion empfaengt Text vom Server
static int netRecv(char* bufAnf, size_t bufRestLen){
		
	int len = recv(socke, bufAnf, bufRestLen,0);
	if(len <= 0) {
		log_printf(LOG_ERROR,"Fehler beim Empfangen der Daten vom Server!\n");
		exit(EXIT_FAILURE);
	}
	return len;
}

//Hilfs-Funktion: noch nicht ausgegebenen Teil des Buffers nach vorne schieben + bufferBenutzt ändern
static int netUpdateBuffer(char* bufferGet, int bufferBenutzt, int bufferEnde ) {
   for(int j=bufferEnde+1, k=0; j<=bufferBenutzt; j++,k++) {
     bufferGet[k] = bufferGet[j];
   }
		return bufferEnde+1;
 }

// Schreibt den Text aus *bufferSend in das Socket
int netWrite(char *bufferSend) {
	int len = strlen(bufferSend);
	if(write(socke, bufferSend, len*sizeof(char)) < 0) {
		log_printf(LOG_ERROR,"Fehler beim Schreiben in den Socket\n");
		return 0;
	}
	log_printf(LOG_DEBUG,"\033[1m\033[36m""C: ""\033[0m""%s",bufferSend); // cyan bold
	//fflush(stdout);
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
    log_printf(LOG_ERROR, "Fehler beim Servernamen\n");
    exit(EXIT_FAILURE);
  }
 
  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    // Socket anlegen
    if ((socke = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      log_printf(LOG_ERROR, "Socket konnte nicht angelegt werden.\n");
        continue;
    }
    // Mit Socket verbinden
    if (connect(socke, p->ai_addr, p->ai_addrlen) == -1) {
      close(socke);
      log_printf(LOG_ERROR, "Verbindung mit Socket fehlgeschlagen.\n");
      continue;
    }
    break;
  }
	if (p == NULL) {
		log_printf(LOG_ERROR, "Client konnte nicht verbunden werden.\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(servinfo); // all done with this structure
	
	return 1;
}

int getSocketFd(){
	return socke;
}

int netDisconnect() {
	close(socke);
	return 0;
}
