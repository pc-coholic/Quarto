#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CLIENTVERSION 1.0

//Funktion prueft, ob nachricht vom Server mit '-' beginnt (wenn ja, wird abgebrochen)
void checkMinus(char *nachricht) {
	if(nachricht[0] == '-') {
		perror("Verbindung wurde nicht akzeptiert");
		exit(EXIT_FAILURE);
	}
}

//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection(int socket, char* gameId) {
  char nachricht[250];
	int len = 0;  

  //get gameserver version
	len = recv(socket, nachricht, sizeof(nachricht),0);
	nachricht[len]='\0';
  printf("S: %s",nachricht);
  //checkMinus(nachricht);
  

  //Client-Version an Server senden
	//snprintf(nachricht,13,"VERSION %f",CLIENTVERSION);
	snprintf(nachricht,13,"VERSION %f",CLIENTVERSION);
	if(write(socket, nachricht, sizeof(nachricht)) < 0) {
		perror("Fehler beim Schreiben in den Socket");
	}
   printf("C: %s\n",nachricht);
   

  //Client-Version vom Server akzeptiert?
	len =  recv(socket, nachricht, sizeof(nachricht),0);
	nachricht[len]='\0';
	printf("S: %s",nachricht);
  //checkMinus(nachricht);
   
   
  //Game_ID an Server senden 
	snprintf(nachricht,15,"ID %s",gameId);
	if( send(socket, nachricht, sizeof(nachricht),0) == -1) {
		perror("Fehler beim Schreiben in den Socket");
	}
	printf("C: %s\n",nachricht);
   
	//Server: Welches Spiel?
	len = recv(socket, nachricht, sizeof(nachricht),0);
	nachricht[len]='\0';
	printf("S: %s",nachricht);
	//checkMinus(nachricht);
}
