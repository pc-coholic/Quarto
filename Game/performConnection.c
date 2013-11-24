#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "network.h"

#define CLIENTVERSION "1.42"
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

<<<<<<< HEAD
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

=======
//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection(char* gameId, int player) {
	char sendText[BUF]; 
	char *getText;
	int len = 0;

  // Server: gameserver version
	netReadLine();
	netUpdateBuffer();

  //Client-Version an Server senden
	snprintf(sendText,23,"VERSION %s\n",CLIENTVERSION);
	len = strlen(sendText);
	netWrite(sendText);

  //Server: Client-Version vom akzeptiert?
	netReadLine();
	netUpdateBuffer();

  //Game_ID an Server senden 
	snprintf(sendText,16,"ID %s\n",gameId);
	len = strlen(sendText);
	netWrite(sendText);
   
	//Server: Welches Spiel?
	getText = netReadLine();
>>>>>>> 8e78d175d795a5fcc0f2fe83b52e6f090530b80e
	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((getText+10),"Quarto")!=0) {
		perror("Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}	
	netUpdateBuffer();

	//Server: Game-Name 
<<<<<<< HEAD
	getFromServer(socket, bufferGet);
	//checkMinus(buffer);
=======
	netReadLine();
	netUpdateBuffer();
>>>>>>> 8e78d175d795a5fcc0f2fe83b52e6f090530b80e

	// Gewuenschte Spielernummer an Server senden 
	// optional Spielernummer angeben
	if (player!='3') {
		snprintf(sendText,15,"PLAYER %c\n", player);
	}
	else {
		snprintf(sendText,10,"PLAYER\n");
	}
<<<<<<< HEAD
	sendToServer(socket, bufferSend);

	//Server: zugeteilte Spielernummer und Name 
	getFromServer(socket, bufferGet);
	//checkMinus(buffer);

	//Server: Spieleranzahl 
	getFromServer(socket, bufferGet);

	//Server: Endplayers 
	getFromServer(socket, bufferGet);
=======
	netWrite(sendText);

	//Server: zugeteilte Spielernummer und Name 
	netReadLine();
	netUpdateBuffer();

	//Server: Spieleranzahl 
	netReadLine();
	netUpdateBuffer();

	//Server: Spieler xy ist (nicht) bereit 
	netReadLine();
	netUpdateBuffer();

	//Server: Endplayers 
	netReadLine();
	netUpdateBuffer();
>>>>>>> 8e78d175d795a5fcc0f2fe83b52e6f090530b80e

	return 0;
}
