#include "spielfeldAusgabe.h"

extern struct shmInfos *shmPtr;

static void printSpaltenNamen(int breite);
static void printMultiZeichen(int breite,char beginEnd,char between);
static void printSteinNummern(int field[], int breite,int currentPlace);

//Spielfeld ausgabe
void printField(int* shmPtr_Sf) {
	int breite= shmPtr->breite;
	int hoehe = shmPtr->hoehe;
	int field[17] = { 9, 7, -1, 8, 3, -1, 2, -1, 15, -1, 6, 4, -1, 11, -1, 10 };
	int currentPlace = 0;
	
	log_printf(LOG_PRINTF,"\n");
	printSpaltenNamen(breite);
	printMultiZeichen(breite,'+','-');
	//Zeilen ausgeben
	while (hoehe) {
		int n=breite;

		if(1) {printSteinNummern(field,breite,currentPlace);}
		/*if(n!=hoehe) { 
			printMultiZeichen(breite,'|',' ');
		}*/

		log_printf(LOG_PRINTF,"%i | ",hoehe);
		//einzelne Steine ausgeben
		while (n) {
			if (field[currentPlace] == -1) {
				for (int i=0; i<breite; i++) {
					log_printf(LOG_PRINTF,"*");
				}
			}
			else {
				printSpielstein(shmPtr_Sf,field[currentPlace]);
			}
			log_printf(LOG_PRINTF," ");
			currentPlace++;
			n--;
		}
		log_printf(LOG_PRINTF,"| %i\n",hoehe);
		hoehe--;
	}

	printMultiZeichen(breite,'+','-');
	printSpaltenNamen(breite);
}

//Spielstein in binärdastellung
void printSpielstein(int* shmPtr_Sf,int i) {
	int breite = shmPtr->breite;
	while (breite) {
		if (i & 1) {
			log_printf(LOG_PRINTF,"\033[41m"" ");
		} else {
			log_printf(LOG_PRINTF,"\033[42m"" ");
		}
	log_printf(LOG_PRINTF,"\033[0m");
	i >>= 1;
	breite--;
	}
}

//die Spaltennamen von A bis ... printen
static void printSpaltenNamen(int breite) {
	char c = 'A';
	while (breite) {
		log_printf(LOG_PRINTF,"    %c",c);
		c++;
		breite--;
	}
	log_printf(LOG_PRINTF,"\n");
}

//die Begrenzungszeilen über und unter dem Spielfeld
static void printMultiZeichen(int breite,char beginEnd,char between) {
	log_printf(LOG_PRINTF,"  %c",beginEnd);
	int i= breite*breite+breite+1;
	while (i) {
		log_printf(LOG_PRINTF,"%c",between);
		i--;
	}
	log_printf(LOG_PRINTF,"%c\n",beginEnd);
}

//Steinnummernausgeben
static void printSteinNummern(int field[],int breite, int currentPlace) {
	int n = breite;
	log_printf(LOG_PRINTF,"  | ""\033[2m");
	while (n) {
		if (field[currentPlace] == -1) {
                               for (int i=0; i<breite; i++) {
                                       log_printf(LOG_PRINTF," ");
                               }
                       }
                       else {
                               log_printf(LOG_PRINTF,"\033[2m""(%i)""\033[0m",field[currentPlace]);
			if(field[currentPlace] < 10) {
				log_printf(LOG_PRINTF," ");
			}
                       }
                       log_printf(LOG_PRINTF," ");
                       currentPlace++;
                       n--;
               }
	log_printf(LOG_PRINTF,"|\n");
	currentPlace = currentPlace - breite;
	n= breite;
}
