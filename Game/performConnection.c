#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CLIENTVERSION 1.0
#define BUF 251
// allgemeine Konvention: Funktionen geben 1 bei Erfolg und 0 bei Fehler zurueck
// bei Funktionen: im if-Zweig Fehlerfall abfragen

//Funktion prueft, ob buffer vom Server mit '-' beginnt (wenn ja, wird abgebrochen)
void checkMinus(char *buffer) {
	if(buffer[0] == '-') {
		perror("Verbindung wurde nicht akzeptiert");
		exit(EXIT_FAILURE);
	}
}

//liest nte Zeile aus dem Buffer
void readNteLine (char *bufferGet, char *line, int lineNumber) {
	int currentLine = 0;
	int i;
	while(currentLine != lineNumber) {
		i=0;
		do {
			line[i] = bufferGet[i];
			i++;
		} while (bufferGet[i] != '\n');
		currentLine++;
	}
}

void newlineToArray (char *bufferGet, char *array[BUF]) {
	
}

// Text vom Server empfangen (und nach Zeilen aufteilen)
int getFromServer(int socket, char *bufferGet){
	int len = recv(socket, bufferGet, sizeof(bufferGet-1),0);
	bufferGet[len]='\0';
	if (len <= 0) {
		return 0;	
	}
  printf("S: %s\n",bufferGet);
  //checkMinus(buffer);
	return 1;	
}
// bufferSend muss mit \n terminiert sein 
int sendToServer(int socket, char *bufferSend) {
	int len = strlen(bufferSend);
	if(write(socket, bufferSend, (len)*sizeof(char)) < 0) {
		perror("Fehler beim Schreiben in den Socket");
		return 0;
	}
  printf("C: %s\n",bufferSend);
	return 1;
}

//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection(int socket, char* gameId, int player) {
	char bufferSend[BUF]; 
	char bufferGet[BUF]; 
	
  //get gameserver version
	getFromServer(socket, bufferGet);

  //Client-Version an Server senden
	snprintf(bufferSend,13,"VERSION %f\n",CLIENTVERSION);
	
	sendToServer(socket, bufferSend);
   

  //Client-Version vom Server akzeptiert?
	getFromServer(socket, bufferGet);
	//checkMinus(buffer);
   

  //Game_ID an Server senden 
	snprintf(bufferSend,16,"ID %s\n",gameId);
	sendToServer(socket, bufferSend);
   
	//Server: Welches Spiel?
	getFromServer(socket, bufferGet);
	//checkMinus(buffer);

	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((bufferGet+10),"Quarto\n")!=0) {
		perror("Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}	

	//Server: Game-Name 
	getFromServer(socket, bufferGet);
	//checkMinus(buffer);

	// Gewuenschte Spielernummer an Server senden 
	// optional Spielernummer angeben
	if (player!='3') {
		snprintf(bufferSend,7,"PLAYER %c", player);
	}
	else {
		snprintf(bufferSend,7,"PLAYER");
	}
	sendToServer(socket, bufferSend);

	//Server: zugeteilte Spielernummer und Name 
	getFromServer(socket, bufferGet);
	//checkMinus(buffer);

	//Server: Spieleranzahl 
	getFromServer(socket, bufferGet);

	//Server: Endplayers 
	getFromServer(socket, bufferGet);

	return 0;
}
