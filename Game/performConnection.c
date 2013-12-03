#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "network.h"
#include "sharedmemory.h"

#define CLIENTVERSION "1.42"
#define BUF 251
// allgemeine Konvention:tismD6nu1.4 Funktionen geben 1 bei Erfolg und 0 bei Fehler zurueck
// bei Funktionen: im if-Zweig Fehlerfall abfragen

int checkAndSendWait(char *getText) {
	char sendText[BUF];
	int erg = 0;
	if(strcmp(getText,"+ WAIT")==0) {
		snprintf(sendText,10,"OKWAIT");
		printf("%s\n",sendText);
		erg = netWrite(sendText);
	}
	return erg;
}

//Funktion prueft, ob buffer vom Server mit '-' beginnt (wenn ja, wird abgebrochen)
void checkMinus(char *buffer) {
	if(buffer[0] == '-') {
		printf("\n%s\n\n",buffer);
		exit(EXIT_FAILURE);
	}
}


//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection(char* gameId, int player, struct shmInfos *shmPtr) {
	char sendText[BUF]; 
	char *getText;

	int i;

  // Server: gameserver version
	netReadLine();

  //Client-Version an Server senden
	snprintf(sendText,23,"VERSION %s\n",CLIENTVERSION);
	netWrite(sendText);

  //Server: Client-Version vom akzeptiert?
	netReadLine();

  //Game_ID an Server senden 
	snprintf(sendText,16,"ID %s\n",gameId);
	netWrite(sendText);
	printf("\nGame ID: %s\n",gameId);
   
	//Server: Welches Spiel?
	getText = netReadLine();
	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((getText+10),"Quarto")!=0) {
		printf("Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}
	printf("%s\n",getText+2);	

	//Server: Game-Name 
	getText = netReadLine();
	printf("Name: %s\n",getText+2);

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
	checkMinus(getText);
	// schöne Ausgabe
	printf("\nYou are Player %i (",getText[6]-'0'+1);
	i=8;
	while(getText[i] != '\0') {
		printf("%c",getText[i]);
		i++;
	}
	printf(")\n");
	
	//Server: Spieleranzahl 
	netReadLine();

	//Server: Spieler xy ist (nicht) bereit 
	netReadLine();
	
	// schöne Ausgabe
	printf("Player %c (",getText[2]);
	i=4;
	while(getText[i+2] != '\0') {
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
	
	//Server: Endplayers 
	printf("S: %s\n",netReadLine());

	//Server: wait
	getText = netReadLine();
	printf("%s\n",getText);
	//checkAndSendWait(getText);

	return 0;
}

