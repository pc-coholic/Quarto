#include "denkmal.h"

char spielzug[10];
extern struct shmInfos *shmPtr;
extern int *shmPtr_Sf;
//extern int *shmPtr_Sf;

static void randomthink();
//berechnet aus dem arrayindex des Feldes den Feldnamen z.B. A1
static void indexToFeld(int index);

void think() {
	
	if(shmPtr_Sf == NULL) {
		shmPtr_Sf = shmSpielfeldAnbinden(shmPtr->shmid_Sf);
		shmDelete(shmPtr->shmid_Sf);
	}

	log_printf(LOG_DEBUG,"fängt an zu denken\n");
	randomthink();
}

void randomthink() {
	int nextStein=0;
	int index=0;
	int len = shmPtr->hoehe * shmPtr->breite;
	//int shmPtr_Sf[16] = { 9, 7, -1, 8, 3, -1, 2, -1, 15, -1, 6, 4, -1, 11, -1, 10 };

	
	//freies Feld suchen und speichern
	do {
		index++;
		if(shmPtr_Sf[index] < 0) {
			break;
		}
	} while(index < len);
	indexToFeld(index);
	spielzug[2] = ',';

	//freien Spielstein suchen und speichern
	while (nextStein < 16) {
		int i;
		if (nextStein != shmPtr->nextStone) {
			
			for(i=0;i<16;i++) {
				if(shmPtr_Sf[i] == nextStein) {
					break;
				}
			}
			if(i==16) {
				break;
			}
		}
		nextStein++;
	}
	sprintf(spielzug+3,"%i",nextStein);
	log_printf(LOG_DEBUG,"Spielzug: %s\n", spielzug);
}

void indexToFeld(int index) {
	//Spalte
	spielzug[0] ='A' +(index % shmPtr->breite);
	//Reihe
	spielzug[1] ='4' -(index/shmPtr->hoehe);

}
