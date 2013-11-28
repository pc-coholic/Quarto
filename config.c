#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct configParameter{
char *hostname;
int portnr;
char *spiel;
};

FILE* openFile(char* name) {
  FILE *datei = NULL;
  
  datei=fopen(dateiName,"r+");
   
  if (datei == NULL) { 
    printf("Fehler beim Oeffnen der Datei!\n");
  }
  
  return datei;
}

// Funktion um die Parameter aus der Datei zu lesen und in die Struktur zu speichern.
struct configParameter getParameter(char* name){
	struct configParameter config;
	FILE *datei = openFile(name);
	
	
}
