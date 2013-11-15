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

//Methode gibt bei fehler 0 zurueck, bei Erfolg 1
int performConnection(int socket, char* gameId) {
  char nachricht[256];
  
  //get gameserver version
  read(socket,nachricht,255);
  printf("%s",nachricht);
  checkMinus(nachricht);
  
  //Client-Version an Server senden
  snprintf(nachricht,13,"VERSION %f",CLIENTVERSION);
  if(write(socket, nachricht, sizeof(nachricht)) < 0) {
      perror("Fehler beim Schreiben in den Socket");
  }
  
  //Client-Version vom Server akzeptiert?
   read(socket,nachricht,255);
   printf("%s",nachricht);
   checkMinus(nachricht);
   
   
   //Game_ID an Server senden
   snprintf(nachricht,30,"ID %s",gameId);
   
    if(write(socket, nachricht, sizeof(nachricht)) < 0) {
      perror("Fehler beim Schreiben in den Socket");
  }
   
   //Server: Welches Spiel?
   read(socket,nachricht,255);
   printf("%s",nachricht);
   checkMinus(nachricht);
}