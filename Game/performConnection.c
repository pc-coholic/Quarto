#include "performConnection.h"
extern struct shmInfos* shmPtr;
extern int *shmPtr_Sf;
// allgemeine Konvention: Funktionen geben 1 bei Erfolg und 0 bei Fehler zurueck
// bei Funktionen: im if-Zweig Fehlerfall abfragen

static void saveField(char *getText);

void sendOkwait() {
	char sendText[BUF];
	snprintf(sendText,10,"OKWAIT\n");
	netWrite(sendText);
	log_printf(LOG_PRINTF,"Warten auf den anderen Spieler\n");
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
	int i=0;
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
	//Naechsten Stein ins SHM schreiben
	int next = atoi(getText+7);
	shmPtr->nextStone = next;
}

void parseField(char* getText) {
	int hoehe;
	int breite;

	//hoehe, breite nur speichen, wenn noch nicht geschehen
	//Spielfeld sharedMem anlegen
	if (shmPtr->breite == 0) {
		
		//ToDo: Spielfeldgroesse korrekt parsen
		int len = strlen(getText);
		char breiteChar[20];
		int i=0;
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
		shmPtr->shmid_Sf = shmid_Sf;
		shmPtr_Sf = shmSpielfeldAnbinden(shmid_Sf);
		shmDelete(shmid_Sf);
		
		shmPtr->breite = breite;
		shmPtr->hoehe = hoehe;
		
		log_printf(LOG_DEBUG,"Hoehe im Shared Memory: %i\n",shmPtr->hoehe);
		log_printf(LOG_DEBUG,"Breite im Shared Memory: %i\n",shmPtr->breite);
	}
	else {
		breite = shmPtr->breite;
		hoehe = shmPtr->hoehe;
	}

	// Spielfeld in array Speichern
	for (int i=0; i<hoehe; i++) {
		getText = netReadLine();
		saveField(getText);
	}

	
//	saveField(shmPtr_Sf,blaText);
	log_printf(LOG_DEBUG,"Spielfeldarray: ");
	for(int i=0; i<16; i++) {
		log_printf(LOG_DEBUG,"%i ",shmPtr_Sf[i]);
	}
	log_printf(LOG_DEBUG,"\n");
	
	log_printf(LOG_PRINTF,"\nNächster Stein: (%i)",shmPtr->nextStone);
	printSpielstein(shmPtr->nextStone);
	log_printf(LOG_PRINTF,"\n");
	printField();

}		

void sendThinking() {
	//Flag setzen und DANN dem Thinker Bescheid geben
	shmPtr->flag = 1;
	connector2thinker(getppid());
	char sendText[BUF];
	snprintf(sendText,12,"THINKING\n");
	netWrite(sendText);
	log_printf(LOG_PRINTF,"\nSpielzug wird berechnet...");
}

void parseGameover(char* getText) {
	//ToDo: Gewinner parsen
  int winnerid = atoi(getText+11);
  if (winnerid - 1 == shmPtr->eigSpielernummer) {
    log_printf(LOG_PRINTF, "\033[1m\033[32m""You, %s, won the game.\n""\033[0m", shmPtr->attr[shmPtr->eigSpielernummer].name);
  } else {
    log_printf(LOG_PRINTF, "\033[1m\033[31m""You lost the game. %s won.\n""\033[0m", shmPtr->attr[winnerid - 1].name);
  }
}

void sendMove(char *spielzug) {
	char sendText[BUF];
	snprintf(sendText,15,"PLAY %s\n", spielzug);
	netWrite(sendText);
	log_printf(LOG_PRINTF,"Done: %s\n",spielzug);
}

//Spielfeld in shm speichern
void saveField(char *getText) {
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
