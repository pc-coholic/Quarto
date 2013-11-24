#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct configParameter{
char *hostname;
int portnr;
char *spiel;
};

FILE* openFile(char* name) {
  char dateiName[256];
  FILE *datei = NULL;
  
  if (name == NULL) {
    datei=fopen("client.conf","r+");
  } else {
    strncpy(dateiName, name, 255*sizeof(char));
	datei=fopen(dateiName,"r+");
  }
 
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