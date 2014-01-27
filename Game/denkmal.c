#include "denkmal.h"
#include <time.h>

char spielzug[10];
extern struct shmInfos *shmPtr;
extern int *shmPtr_Sf;
int besetzteSteine[16];

static void thinkbetter();
static int randomStone(int stones[]);
static int randomField(int felder[]);
//berechnet aus dem arrayindex des Feldes den Feldnamen z.B. A1
static void saveField(int index);
// 1 heißt Eigenschaft stimmt überein, 0 heißt keine stimmt überein
static int compareFourStones(int a, int b, int c, int d);
static int cmpTwoInvertBits(int a, int b);
static int checkFreiesFeld(int a, int b, int c, int d);
static int checkReihe(int a, int b, int c, int d, int felder[],int badStones[]);

void think() {
	int groesse = shmPtr->breite * shmPtr->hoehe;
	
	// shm für Spielfeld attachen
	if(shmPtr_Sf == NULL) {
		shmPtr_Sf = shmSpielfeldAnbinden(shmPtr->shmid_Sf);
		shmDelete(shmPtr->shmid_Sf);

	}
	// Welche Steine sind besetzt?
	for(int i=0;i<groesse;i++) {
		besetzteSteine[i]=0;
		for(int j=0;j<groesse;j++) {
			if (shmPtr_Sf[j] ==i) {
				besetzteSteine[i]=1;
				break;
			} 
		}
	}

	log_printf(LOG_DEBUG,"fängt an zu denken\n");

	thinkbetter();

}

void thinkbetter() {
	int besetzteFelder[16]={0};
	int guteFelder[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	int spielsteineSchlecht[16]={0};
	int frei = -1;
	int stein;
	int isThereAStone=0;
	int isThereAField=0;
	srand(time(NULL));

	besetzteSteine[shmPtr->nextStone] = 1;
	
	//benutze Spielfelder und Steine kopieren
	for(int i=0; i<16; i++) {
		spielsteineSchlecht[i]=besetzteSteine[i];
		if (shmPtr_Sf[i] >-1) {
			besetzteFelder[i] =1;
		}
	}

	// Zeilen auf Eigenschaften prüfen
	if ((frei = checkReihe(0,1,2,3,besetzteFelder,spielsteineSchlecht))!= 16){
	   if (frei >0) guteFelder[frei]=0; //gefährliche Felder speichern
	   if ((frei = checkReihe(4,5,6,7,besetzteFelder,spielsteineSchlecht))!= 16){
	      if (frei >0) guteFelder[frei]=0;
	      if ((frei = checkReihe(8,9,10,11,besetzteFelder,spielsteineSchlecht))!= 16){
	         if (frei >0) guteFelder[frei]=0;
	         if ((frei = checkReihe(12,13,14,15,besetzteFelder,spielsteineSchlecht))!= 16){
	            if (frei >0) guteFelder[frei]=0;

	// Spalten prüfen
	            if ((frei = checkReihe(0,4,8,12,besetzteFelder,spielsteineSchlecht))!= 16){
	               if (frei >0) guteFelder[frei]=0;
	               if ((frei = checkReihe(1,5,9,13,besetzteFelder,spielsteineSchlecht))!= 16){
	                  if (frei >0) guteFelder[frei]=0;
	                  if ((frei = checkReihe(2,6,10,14,besetzteFelder,spielsteineSchlecht))!= 16){
	                     if (frei >0) guteFelder[frei]=0;
	                     if ((frei = checkReihe(3,7,11,15,besetzteFelder,spielsteineSchlecht))!= 16){
	                        if (frei >0) guteFelder[frei]=0;

	// Diagonalen prüfen
	                        if ((frei = checkReihe(0,5,10,15,besetzteFelder,spielsteineSchlecht))!= 16){
	                           if (frei >0) guteFelder[frei]=0;
	                           frei = checkReihe(3,6,9,12,besetzteFelder,spielsteineSchlecht);
	                           if (frei >0) guteFelder[frei]=0;
	        }
               }
              }
             }
            }
           }
	  }
	 }
	}

	// Gibt es ein Reihe mit drei Steinen?
	log_printf(LOG_DEBUG,"guteFelder: ");
	for (int i=0; i<16; i++) {
		log_printf(LOG_DEBUG,"%i ",guteFelder[i]);
		if (guteFelder[i] == 0) {
			isThereAField = 1;
		} 	
	}
	log_printf(LOG_DEBUG,"\n");
	log_printf(LOG_DEBUG,"isThereAField: %i\n",isThereAField);

	// wenn nicht gewonnen, dann hier saveField, sonst siehe checkReihe()
	if(frei < 16) {
		if(isThereAField==1){
			log_printf(LOG_DEBUG,"there is\n");
			saveField(randomField(guteFelder));
		} else{
			log_printf(LOG_DEBUG,"Nope, there isn't\n");
			saveField(randomField(besetzteFelder));
		}
	}

	spielzug[2] = ',';

	// Spielstein festlegen
	
	// Gibt es einen Stein, der keine Gefahr darstellt?
	log_printf(LOG_DEBUG,"schlechte Steine: ");
	for (int i=0; i<16; i++) {
		log_printf(LOG_DEBUG,"%i ",spielsteineSchlecht[i]);
		if (spielsteineSchlecht[i] == 0) {
			isThereAStone = 1;
		} 	
	}
	log_printf(LOG_DEBUG,"\n");
	log_printf(LOG_DEBUG,"isThereAStone: %i\n",isThereAStone);

	//wenn nicht, dann randomField()
	if(isThereAStone==1){
		log_printf(LOG_DEBUG,"there is\n");
		stein = randomStone(spielsteineSchlecht);
	} else{
		log_printf(LOG_DEBUG,"Nope, there isn't\n");
		stein = randomStone(besetzteSteine);
	}
	log_printf(LOG_DEBUG,"stein: %i\n",stein);
	sprintf(spielzug+3,"%i",stein);
	log_printf(LOG_DEBUG,"Spielzug: %s\n",spielzug);
}
//gibt Feld zurück
int randomField(int felder[]) {
	int len = shmPtr->hoehe * shmPtr->breite; 
	// Random Int von 0 bis Groesse des Spielfeldes
	
	int anzahlBesetzt = 0;
	
	for(int i = 0; i<len;i++){
		anzahlBesetzt += felder[i];
	}
	int freieFelder = len-anzahlBesetzt;
	int freieFelderArray[freieFelder];
	int j = 0;
	for(int i = 0; i<len;i++){
		if(felder[i] == 0){
			freieFelderArray[j] = i;
			j++;
		}	
	}
	//freies Feld suchen und speichern
	int random = rand() % freieFelder;
	int index = freieFelderArray[random];

	log_printf(LOG_DEBUG,"Tolles freies Feld: %i\n",index);
	return index;
}

// gibt möglichen Spielstein zurück
int randomStone(int stones[]) {
	//freien Spielstein suchen und speichern
	//waehlt nextStein random aus 0 und 15
	int nextStein = rand() % 16;

while(1){	
	if (stones[nextStein] ==0){
			log_printf(LOG_DEBUG,"Naechster Stein: %i\n",nextStein);
			return nextStein;
		}
		else{
		nextStein = rand() % 16;
	}
	}
	log_printf(LOG_DEBUG,"Naechster Stein: %i\n",nextStein);
	return -1;
}

void saveField(int index) {
	//Spalte
	spielzug[0] ='A' +(index % shmPtr->breite);
	//Reihe
	spielzug[1] ='4' -(index/shmPtr->hoehe);

}
// Steinnummern angeben
int compareFourStones(int a, int b, int c, int d) {
	//gleiche Eigenschaft = 1
	int zwerg;
	if ((a&b&c&d) >0) {
		return 1;
	}
	
	// falls gleiche Eigenschaft = 0 (aber es ist eine vorhanden
	a = ~a;
	b = ~b;
	c = ~c;
	d = ~d;
	
	if (cmpTwoInvertBits(a,b) ==1) {
		zwerg = a & b;
		if (cmpTwoInvertBits(zwerg,c) == 1) {
			zwerg = zwerg & c;
			if (cmpTwoInvertBits(zwerg,d) ==1) {
				return 1;
			}
		}
	}
	return 0;
}
// steinnummern angeben
int cmpTwoInvertBits(int a, int b) {
	int i = 4;
	int zwerg = a&b;
	while(i) {
		if(zwerg & 1) {
			return 1;
		}
		zwerg >>= 1;
		i--;
	}
	return 0;
}
//indizis/Feldnummer angeben
int checkFreiesFeld(int a, int b, int c, int d) {
	if (shmPtr_Sf[a] <0 ) return a;
	if (shmPtr_Sf[b] <0 ) return b;
	if (shmPtr_Sf[c] <0 ) return c;
	if (shmPtr_Sf[d] <0 ) return d;
	return 0;
}
// indizies/Feldnummer angeben
// return: 16 bei gewonnen(dann wird auch gleich Feld in Spielzug gespeichert), -1 bei weniger als drei Steine, sonst das freie Feld
int checkReihe(int a, int b, int c, int d, int felder[], int badStones[]) {
	if (felder[a]+felder[b]+felder[c]+felder[d] == 3) {
		int frei = checkFreiesFeld(a,b,c,d);
		int stein1,stein2,stein3,stein4;
		stein1 =0; stein2 =0; stein3 =0; stein4 =0;
		log_printf(LOG_DEBUG,"freies Feld: %i\n",frei);
		// freies Feld wird durch nextStone ersetzt
		if (frei == a) {
			stein1 = shmPtr->nextStone;
			stein2 = shmPtr_Sf[b];
			stein3 = shmPtr_Sf[c];
			stein4 = shmPtr_Sf[d];
		}
		else if (frei == b) {
			stein1 = shmPtr_Sf[a];
			stein2 = shmPtr->nextStone;
			stein3 = shmPtr_Sf[c];
			stein4 = shmPtr_Sf[d];
		}
		else if (frei == c) {
			stein1 = shmPtr_Sf[a];
			stein2 = shmPtr_Sf[b];
			stein3 = shmPtr->nextStone;
			stein4 = shmPtr_Sf[d];
		}
		else if (frei == d) {
			stein1 = shmPtr_Sf[a];
			stein2 = shmPtr_Sf[b];
			stein3 = shmPtr_Sf[c];
			stein4 = shmPtr->nextStone;
		}

		if (compareFourStones(stein1,stein2,stein3,stein4) == 1) {
			//Yeah gewonnen!
			saveField(frei);
			log_printf(LOG_DEBUG,"gewonnen!\n");
			return 16;
		}
		
		// Gibt es Steine, die man auf das Feld nicht setzen sollte?
		for(int i=0;i<16;i++) {
			if (frei == a) {
				stein1 = i;
				stein2 = shmPtr_Sf[b];
				stein3 = shmPtr_Sf[c];
				stein4 = shmPtr_Sf[d];
			}
			else if (frei == b) {
				stein1 = shmPtr_Sf[a];
				stein2 = i;
				stein3 = shmPtr_Sf[c];
				stein4 = shmPtr_Sf[d];
			}
			else if (frei == c) {
				stein1 = shmPtr_Sf[a];
				stein2 = shmPtr_Sf[b];
				stein3 = i;
				stein4 = shmPtr_Sf[d];
			}
			else if (frei == d) {
				stein1 = shmPtr_Sf[a];
				stein2 = shmPtr_Sf[b];
				stein3 = shmPtr_Sf[c];
				stein4 = i;
			}

			if (compareFourStones(stein1,stein2,stein3,stein4) ==1) {
				badStones[i] = 1;
			}
		}
		return frei;
	}
	return -1;
	
}
