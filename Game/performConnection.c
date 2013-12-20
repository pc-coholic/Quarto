#include "performConnection.h"
extern struct shmInfos* shmPtr;
// allgemeine Konvention: Funktionen geben 1 bei Erfolg und 0 bei Fehler zurueck
// bei Funktionen: im if-Zweig Fehlerfall abfragen

static void saveField(int* shmPtr_Sf, char *getText);

void sendOkwait() {
	char sendText[BUF];
	snprintf(sendText,10,"OKWAIT\n");
	netWrite(sendText);
}

//Funktion prueft, ob buffer vom Server mit '-' beginnt (wenn ja, wird abgebrochen)
void checkMinus(char *buffer) {
	if(buffer[0] == '-') {
		log_printf(LOG_PRINTF, "\n\n%s\n\n",buffer);
		exit(EXIT_FAILURE);
	}
}

void sendVersion() {
	char sendText[BUF];
	snprintf(sendText,23,"VERSION %s\n",CLIENTVERSION);
	netWrite(sendText);
}

void sendGameId(char* gameId) {
	char sendText[BUF];
	snprintf(sendText,16,"ID %s\n",gameId);
	netWrite(sendText);
	log_printf(LOG_PRINTF,"Game ID: %s\n",gameId);
}

void parseGamekind(char* getText, struct shmInfos *shmPtr) {
	//Server: Welches Spiel?
	log_printf(LOG_PRINTF,"%s\n\n",getText+2);	
	// Spielname in shmInfo eintragen
	strcpy(shmPtr->spielname,getText);  // Tragen wir hier in den Spielnamen + PLAYING Quarto ein? 
	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((getText+10),"Quarto")!=0) {
		log_printf(LOG_ERROR,"Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}

	//Server: Game-Name 
	getText = netReadLine();
	log_printf(LOG_PRINTF,"Name: %s\n",getText+2);
}

void sendPlayer(int player, struct shmInfos *shmPtr) {
	char sendText[BUF]; 
	char *getText;
	int i;
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
}

void parseMovetimeout(char* getText, struct shmInfos *shmPtr) {
	//ToDo: Movetimeout ins SHM schreiben?
	int time = atoi(getText+7);
	shmPtr->moveTimeout = time;
}

void parseNext(char* getText, struct shmInfos *shmPtr) {
	//ToDo: Naechsten Stein ins SHM schreiben
	int next = atoi(getText+7);
	shmPtr->nextStone = next;
}

int* parseField(char* getText) {
	int hoehe;
	int breite;
	int *shmPtr_Sf = NULL;

	//hoehe, breite nur speichen, wenn noch nicht geschehen
	//Spielfeld sharedMem anlegen
	if (shmPtr->breite == 0) {
		
		//ToDo: Spielfeldgroesse korrekt parsen
		int len = strlen(getText);
		char breiteChar[20];
		int i;
		while (getText[i] != ',' && i<len-1) {
			breiteChar[i] = getText[i];
			i++;
		}	
		breiteChar[i] = '\0';
		int lenB = strlen(breiteChar);	
		hoehe = atoi(getText+lenB+1);
		breite = atoi(breiteChar+8);
		
		log_printf(LOG_PRINTF,"Breite: %i\n",breite);
		log_printf(LOG_PRINTF,"Hoehe: %i\n",hoehe);
		
		// Shared Memory Bereich fuer das Spielfeld anlegen
		int shmid_Sf = shmSegment(hoehe*breite*sizeof(int));
		shmPtr_Sf = shmSpielfeldAnbinden(shmid_Sf);
		shmDelete(shmid_Sf);
		
		shmPtr->breite = breite;
		shmPtr->hoehe = hoehe;
		
		log_printf(LOG_DEBUG,"Hoehe im Shared Memory: %i\n",shmPtr->hoehe);
		log_printf(LOG_DEBUG,"Breite im Shared Memory: %i\n",shmPtr->breite);
	}

	// Spielfeld in array Speichern
	for (int i=0; i<hoehe; i++) {
		getText = netReadLine();
		saveField(shmPtr_Sf,getText);
		
	}

	
//	saveField(shmPtr_Sf,blaText);
	log_printf(LOG_DEBUG,"Spielfeldarray: ");
	for(int i=0; i<16; i++) {
		log_printf(LOG_DEBUG,"%i ",shmPtr_Sf[i]);
	}
	log_printf(LOG_DEBUG,"\n");

	printField(shmPtr_Sf);

	return shmPtr_Sf;
}		

void sendThinking() {
	char sendText[BUF];
	snprintf(sendText,12,"THINKING\n");
	netWrite(sendText);
	//Flag setzen und dem Thinker Bescheide geben
	shmPtr->flag = 1;
	connector2thinker(getppid());
}

void parseGameover(char* getText) {
	//ToDo: Gewinner parsen
	log_printf(LOG_PRINTF,"You lost the game. Or won. Whatever\n");
}

void sendMove(char* block, int nextblock) {
	char sendText[BUF];
	snprintf(sendText,15,"PLAY %s,%i\n", block, nextblock);
	netWrite(sendText);
}

//Spielfeld in shm speichern
void saveField(int* shmPtr_Sf, char *getText) {
        int breite = shmPtr->breite;
	char charHelp[255];
        int i = 2;
        int j =0;
	while (getText[i] != ' ') {
		charHelp[j] = getText[i];
		j++;
		i++;
	}
	charHelp[j] = '\0';
	i++;	
	int zeile = breite-atoi(charHelp);
        int currentField = zeile*breite;
//printf("breite: %i\nzeile: %i\ncurrent: %i\n", breite,zeile,currentField);
	while (breite) {
                if (getText[i] == '*') {
                        shmPtr_Sf[currentField]= -1;
			i++;
		}
		else if(getText[i] !=' ') {
			j=0;
			while(getText[i] != ' ') {
				charHelp[j] = getText[i];
				j++;
				i++;
			}
			charHelp[j] = '\0';

			shmPtr_Sf[currentField] = atoi(charHelp);
		}
		currentField++;
		i++;
		breite--;
	}
}
