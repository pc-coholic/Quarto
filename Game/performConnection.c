#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CLIENTVERSION "1.42"
#define BUF 251
// allgemeine Konvention: Funktionen geben 1 bei Erfolg und 0 bei Fehler zurueck
// bei Funktionen: im if-Zweig Fehlerfall abfragen


int bufferBenutzt = 0;//bis zu welcher Stelle von buffer eingelesen wurde
int bufferEnde = 0; //Stelle von newline im buffer

//Funktion prueft, ob buffer vom Server mit '-' beginnt (wenn ja, wird abgebrochen)
void checkMinus(char *buffer) {
	if(buffer[0] == '-') {
		perror("Verbindung wurde nicht akzeptiert");
		exit(EXIT_FAILURE);
	}
}


//Funktion empfaengt Text vom Server
int empfangen(int socket, char *buffer){
	int len = recv(socket, (buffer + bufferBenutzt), BUF-1,0);
	bufferBenutzt += len;
	return 1;
}


//Funktion: noch nicht ausgegebenen Teil des Buffers nach vorne schieben + bufferBenutzt ändern
int updateBuffer(char *buffer) {
	for(int j=bufferEnde+1, k=0; j<=bufferBenutzt; j++,k++) {
		buffer[k] = buffer[j];
	}
	bufferBenutzt -= bufferEnde+1;	
	return 1;
}

/* Funktion gibt eine einzelne Zeile aus */
int zeileChecken(int socket, char *buffer) {
	//ist eine Zeile im Buffer? -> ausgeben
	for (int i=0; i<bufferBenutzt; i++) {
		if(buffer[i]=='\n') {
			buffer[i] = '\0';	
			bufferEnde = i;
  		printf("S: %s\n",buffer);
			fflush(stdout);
			return 1;
		}
	}
	//ansonsten recv vom Server
	empfangen(socket,buffer);
	zeileChecken(socket, buffer);
	return 0;
}


/* Funktionen sendet Text an Server 
 bufferSend muss mit \n terminiert sein */
int sendToServer(int socket, char *bufferSend) {
	int len = strlen(bufferSend);
	if(write(socket, bufferSend, len*sizeof(char)) < 0) {
		perror("Fehler beim Schreiben in den Socket");
		return 0;
	}
	printf("C: %s\n",bufferSend);
	fflush(stdout);
	return 1;
}

//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection(int socket, char* gameId, int player) {
	char bufferSend[BUF]; 
	char bufferGet[BUF]; 
	int len = 0;  

  // Server: gameserver version
	zeileChecken(socket, bufferGet);
	updateBuffer(bufferGet);

  //Client-Version an Server senden
	snprintf(bufferSend,23,"VERSION %s\n",CLIENTVERSION);
	len = strlen(bufferSend);
	sendToServer(socket, bufferSend);
   

  //Server: Client-Version vom akzeptiert?
	zeileChecken(socket, bufferGet);
	updateBuffer(bufferGet);

  //Game_ID an Server senden 
	snprintf(bufferSend,16,"ID %s\n",gameId);
	len = strlen(bufferSend);
	sendToServer(socket, bufferSend);
   
	//Server: Welches Spiel?
	zeileChecken(socket, bufferGet);
	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((bufferGet+10),"Quarto")!=0) {
		perror("Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}	
	updateBuffer(bufferGet);

	//Server: Game-Name 
	zeileChecken(socket, bufferGet);
	updateBuffer(bufferGet);

	// Gewuenschte Spielernummer an Server senden 
	// optional Spielernummer angeben
	if (player!='3') {
		snprintf(bufferSend,15,"PLAYER %c\n", player);
	}
	else {
		snprintf(bufferSend,10,"PLAYER\n");
	}
	sendToServer(socket, bufferSend);

	//Server: zugeteilte Spielernummer und Name 
	zeileChecken(socket, bufferGet);
	updateBuffer(bufferGet);

	//Server: Spieleranzahl 
	zeileChecken(socket, bufferGet);
	updateBuffer(bufferGet);

	//Server: Spieler xy ist (nicht) bereit 
	zeileChecken(socket, bufferGet);
	updateBuffer(bufferGet);

	//Server: Endplayers 
	zeileChecken(socket, bufferGet);
	updateBuffer(bufferGet);

	return 0;
}
