#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "network.h"
#include "logger.h"
#include "sharedmemory.h"

#define CLIENTVERSION "1.42"
#define BUF 251
// allgemeine Konvention: Funktionen geben 1 bei Erfolg und 0 bei Fehler zurueck
// bei Funktionen: im if-Zweig Fehlerfall abfragen

int checkAndSendWait(char *getText) {
	char sendText[BUF];
	int erg = 0;
	if(strcmp(getText,"+ WAIT")==0) {
		snprintf(sendText,10,"OKWAIT");
		//log_more_info(l,"%s\n",sendText);
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
int performConnection(l, char* gameId, int player, struct shmInfos *shmPtr) {
	char sendText[BUF]; 
	char *getText;

	int i;

	log_info(l, "\n\n");
  // Server: gameserver version
	log_more_info(l,"S: %s\n",netReadLine());

  //Client-Version an Server senden
	snprintf(sendText,23,"VERSION %s\n",CLIENTVERSION);
	log_more_info(l,"C: %s",sendText);
	netWrite(sendText);
	
  //Server: Client-Version vom akzeptiert?
	log_more_info(l,"S: %s\n", netReadLine());

  //Game_ID an Server senden 
	snprintf(sendText,16,"ID %s\n",gameId);
	netWrite(sendText);
	log_info(l,"Game ID: %s\n",gameId);
   
	//Server: Welches Spiel?
	getText = netReadLine();
	// Spielname in shmInfo eintragen
	strcpy(shmPtr->spielname,getText);
	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((getText+10),"Quarto")!=0) {
		log_error(l,"Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}
	log_info(l,"%s\n",getText+2);	

	//Server: Game-Name 
	getText = netReadLine();
	log_info(l,"Name: %s\n\n",getText+2);

	// Gewuenschte Spielernummer an Server senden 
	// optional Spielernummer angeben
	if (player!='3') {
		snprintf(sendText,15,"PLAYER %c\n", player);
	}
	else {
		snprintf(sendText,10,"PLAYER\n");
	}
	log_more_info(l,"C: %s", sendText);
	netWrite(sendText);

	//Server: zugeteilte Spielernummer und Name 
	getText = netReadLine();
	checkMinus(getText);
	// Spielernummer in shmInfo eintragen
	shmPtr->eigSpielernummer = getText[6]-'0';
	//printf("eigSpielernummer = %i",shmPtr->eigSpielernummer);
	//struct PlayerAttr befuellen fuer unseren Client 
	shmPtr->attr[shmPtr->eigSpielernummer].spielerNr = shmPtr->eigSpielernummer;
	strcpy(shmPtr->attr[shmPtr->eigSpielernummer].name,getText+8);
	shmPtr->attr[shmPtr->eigSpielernummer].registered = 1;
	
	// Ausgabe Versuch Client Spieler
	printf("SpielernNr Client %i\n",shmPtr->attr[shmPtr->eigSpielernummer].spielerNr);
	printf("Spielername Client %s\n",shmPtr->attr[shmPtr->eigSpielernummer].name);
	printf("Spieler Registered Client %i\n",shmPtr->attr[shmPtr->eigSpielernummer].registered);	
	
	
	// schöne Ausgabe
	log_info(l,"You are Player %i (",getText[6]-'0'+1);
	i=8;
	while(getText[i] != '\0') {
		log_info(l,"%c",getText[i]);
		i++;
	}
	log_info(l,")\n");
	
	//Server: Spieleranzahl 
	getText = netReadLine();
	log_more_info(l,"S: %s\n", getText);
	// Spieleranzahl in shmInfo eintragen
	shmPtr->anzahlSpieler = getText[8]-'0';

	//Server: Spieler xy ist (nicht) bereit 
	getText = netReadLine();
	int anzSp = (getText[2]-'0')-1;
	// Anderer Spieler vom Server in shm Struktur schreiben
	shmPtr->attr[anzSp].spielerNr = anzSp;
	strncpy(shmPtr->attr[anzSp].name,getText+4,strlen(getText+4)-2);
	shmPtr->attr[anzSp].name[strlen(getText+4)-2] = '\0';
	shmPtr->attr[anzSp].registered = getText[strlen(getText)-1]-'0';
	// Ausgabe Versuch:
	
	printf("SpielernNr Server %i\n",shmPtr->attr[anzSp].spielerNr);
	printf("Spielername Server %s\n",shmPtr->attr[anzSp].name);
	printf("Spieler Registered Server %i\n",shmPtr->attr[anzSp].registered);	
	
	// schöne Ausgabe
	log_info(l,"Player %c (",getText[2]);
	i=4;
	while(getText[i+2] != '\0') {
		log_info(l,"%c",getText[i]);
		i++;
	}
	log_info(l,") is ");
	if (getText[i+1] == '1') {
		log_info(l,"ready\n\n");
	} else
	{
		log_info(l,"not ready\n\n");
	}
	
	//Server: Endplayers 
	log_more_info(l,"S: %s\n",netReadLine());


	return 0;
}

