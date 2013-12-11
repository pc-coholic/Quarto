#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "network.h"
#include "logger.h"
#include "sharedmemory.h"
#include "denkmal.h"
#include "prozessSync.h"

#define CLIENTVERSION "1.42"
#define BUF 251
// allgemeine Konvention: Funktionen geben 1 bei Erfolg und 0 bei Fehler zurueck
// bei Funktionen: im if-Zweig Fehlerfall abfragen

int checkAndSendWait(char *getText) {
	char sendText[BUF];
	int erg = 0;
	if(strcmp(getText,"+ WAIT")==0) {
		snprintf(sendText,10,"OKWAIT");
		log_printf(LOG_DEBUG,"%s\n",sendText);
		erg = netWrite(sendText);
	}
	return erg;
}

//Funktion prueft, ob buffer vom Server mit '-' beginnt (wenn ja, wird abgebrochen)
void checkMinus(char *buffer) {
	if(buffer[0] == '-') {
		log_printf(LOG_PRINTF, "\n\n%s\n\n",buffer);
		exit(EXIT_FAILURE);
	}
}


//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection( char* gameId, int player, struct shmInfos *shmPtr) {
	char sendText[BUF]; 
	char *getText;

	int i;

	log_printf(LOG_PRINTF, "\n");
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
	log_printf(LOG_PRINTF,"Game ID: %s\n",gameId);
   
	//Server: Welches Spiel?
	getText = netReadLine();
	log_printf(LOG_PRINTF,"%s\n\n",getText+2);	
	// Spielname in shmInfo eintragen
	strcpy(shmPtr->spielname,getText);
	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((getText+10),"Quarto")!=0) {
		log_printf(LOG_ERROR,"Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}

	//Server: Game-Name 
	getText = netReadLine();
	log_printf(LOG_PRINTF,"Name: %s\n",getText+2);

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
	// Spielernummer in shmInfo eintragen
	shmPtr->eigSpielernummer = getText[6]-'0';
	//struct PlayerAttr befuellen fuer unseren Client 
	shmPtr->attr[shmPtr->eigSpielernummer].spielerNr = shmPtr->eigSpielernummer;
	strcpy(shmPtr->attr[shmPtr->eigSpielernummer].name,getText+8);
	shmPtr->attr[shmPtr->eigSpielernummer].registered = 1;
	
	// Ausgabe Versuch Client Spieler
	/*log_printf(LOG_DEBUG,"SpielernNr Client %i\n",shmPtr->attr[shmPtr->eigSpielernummer].spielerNr);
	log_printf(LOG_DEBUG,"Spielername Client %s\n",shmPtr->attr[shmPtr->eigSpielernummer].name);
	log_printf(LOG_DEBUG,"Spieler Registered Client %i\n",shmPtr->attr[shmPtr->eigSpielernummer].registered);*/
	
	
	// schöne Ausgabe
	log_printf(LOG_PRINTF,"You are Player %i (",getText[6]-'0'+1);
	i=8;
	while(getText[i] != '\0') {
		log_printf(LOG_PRINTF,"%c",getText[i]);
		i++;
	}
	log_printf(LOG_PRINTF,")\n");
	
	log_printf(LOG_PRINTF,"\n");
	//Server: Spieleranzahl 
	getText = netReadLine();
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
	/*
	log_printf(LOG_DEBUG,"SpielernNr Server %i\n",shmPtr->attr[anzSp].spielerNr);
	log_printf(LOG_DEBUG,"Spielername Server %s\n",shmPtr->attr[anzSp].name);
	log_printf(LOG_DEBUG,"Spieler Registered Server %i\n",shmPtr->attr[anzSp].registered);	*/
	
	// schöne Ausgabe
	log_printf(LOG_PRINTF,"Player %c (",getText[2]);
	i=4;
	while(getText[i+2] != '\0') {
		log_printf(LOG_PRINTF,"%c",getText[i]);
		i++;
	}
	log_printf(LOG_PRINTF,") is ");
	if (getText[i+1] == '1') {
		log_printf(LOG_PRINTF,"ready\n\n");
	} else
	{
		log_printf(LOG_PRINTF,"not ready\n\n");
	}

	//Server: Endplayers 
	return 0;
}

