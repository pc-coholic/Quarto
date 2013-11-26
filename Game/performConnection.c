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


void newlineToArray (char *bufferGet, char *array[BUF]) {
	
}

//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection(char* gameId, int player) {
	char sendText[BUF]; 
	char *getText;
	int len = 0;

	int i;

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
	printf("\nGame ID: %s\n",gameId);
   
	//Server: Welches Spiel?
	getText = netReadLine();
	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((getText+10),"Quarto")!=0) {
		perror("Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}
	printf("%s\n",getText+2);	
	netUpdateBuffer();

	//Server: Game-Name 
	getText = netReadLine();
	printf("Name: %s\n",getText+2);
	netUpdateBuffer();

	// Gewuenschte Spielernummer an Server senden 
	// optional Spielernummer angeben
	if (player!='3') {
		snprintf(sendText,15,"PLAYER %c\n", player);
	}
	else {
		snprintf(sendText,10,"PLAYER\n");
	}
	netWrite(sendText);

	//Server: zugeteilte Spielernummer und Name 
	getText = netReadLine();
	
	// schöne Ausgabe
	printf("\nYou are Player %i (",getText[6]-'0'+1);
	i=8;
	while(getText[i] != '\0') {
		printf("%c",getText[i]);
		i++;
	}
	printf(")\n");
	//
	netUpdateBuffer();
	
	//Server: Spieleranzahl 
	netReadLine();
	netUpdateBuffer();

	//Server: Spieler xy ist (nicht) bereit 
	netReadLine();
	
	// schöne Ausgabe
	printf("Player %c (",getText[2]);
	i=4;
	while(getText[i] != ' ') {
		printf("%c",getText[i]);
		i++;
	}
	printf(") is ");
	if (getText[i+1] == '1') {
		printf("ready\n\n");
	} else
	{
		printf("not ready\n\n");
	}
	//
	netUpdateBuffer();
	
	//Server: Endplayers 
	netReadLine();
	netUpdateBuffer();

	return 0;
}
