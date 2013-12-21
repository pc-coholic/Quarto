#include "denkmal.h"

extern struct shmInfos *shmPtr;
//extern int *shmPtr_Sf;

static void randomthink(char *spielzug);
//berechnet aus dem arrayindex des Feldes den Feldnamen z.B. A1
static void indexToFeld(char *feld,int index);

void think() {
	randomthink(shmPtr->spielzug);
	log_printf(LOG_DEBUG,"Spielzug im SharedMem: %s\n",shmPtr->spielzug);
}

void randomthink(char *spielzug) {
	int nextStein=0;
	int index=0;
	int len = shmPtr->hoehe * shmPtr->breite;
	int shmPtr_Sf[16] = { 9, 7, -1, 8, 3, -1, 2, -1, 15, -1, 6, 4, -1, 11, -1, 10 };


	//freies Feld suchen und speichern
	do {
		index++;
		if(shmPtr_Sf[index] < 0) {
			break;
		}
	} while(index < len);
	indexToFeld(spielzug,index);
	spielzug[2] = ',';

	//freien Spielstein suchen und speichern
	while (nextStein < 16) {
		int i;
		for(i=0;i<16;i++) {
			if(shmPtr_Sf[i] == nextStein) {
				break;
			}
		}
		if(i==16) {
			break;
		}
		nextStein++;
	}
	sprintf(spielzug+3,"%i",nextStein);
}

void indexToFeld(char *feld,int index) {
	//Spalte
	feld[0] ='A' +(index % shmPtr->breite);
	//Reihe
	feld[1] ='4' -(index/shmPtr->hoehe);

}
