#include "ausgabe.h"

static void printSpaltenNamen(int breite);
static void printMultiZeichen(int breite,char beginEnd,char between);

static int shmbreite= 4;

//Spielfeld ausgabe
void printField() {
	int breite= shmbreite;
	int hoehe = 4;
	int field[17] = { 9, 7, -1, 8, 3, -1, 2, -1, 15, -1, 6, 4, -1, 11, -1, 10 };
	int currentPlace = 0;
	
	log_printf(LOG_PRINTF,"\n");

	printSpaltenNamen(breite);
	printMultiZeichen(breite,'+','-');
	//Zeilen ausgeben
	while (hoehe) {
		int n=breite;
		if(n!=hoehe) { 
			printMultiZeichen(breite,'|',' ');
		}
		log_printf(LOG_PRINTF,"%i | ",hoehe);
		//einzelne Steine ausgeben
		while (n) {
			if (field[currentPlace] == -1) {
				for (int i=0; i<breite; i++) {
					log_printf(LOG_PRINTF,"*");
				}
			}
			else {
				printSpielstein(field[currentPlace]);
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
void printSpielstein(int i) {
	int breite = shmbreite;
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
