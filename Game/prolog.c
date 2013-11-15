#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define GAMEKINDNAME "Quarto"
#define PORTNUMMER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"

int performConnection(int socket, char* gameId);


int main(int argc, char* argv[]) {
  
  int socke;
  char gameId[11];
  struct sockaddr_in address;
  struct sockaddr_in *h;
  struct addrinfo *result;
 
  if (argc < 2) {
    perror("keine Game-Id angegeben");
  }
  
  //eingelesene GameId speichern
  strcpy(gameId,argv[1]);
  
  //Socket anlegen
   if( (socke = socket(PF_INET, SOCK_STREAM, 0)) == 0) {
     perror("Socket konnte nicht angelegt werden.");
   }
   
   address.sin_family = AF_INET;
   address.sin_port = htons(PORTNUMMER);
   
   //SERVER-Adresse in IP-Adresse umwandeln
   if( (getaddrinfo(HOSTNAME, NULL , NULL, &result)) < 0) {
    perror("Fehler bei Serveraddresse"); 
   }
   h = (struct sockaddr_in*) result->ai_addr;
    inet_ntoa(h->sin_addr);
    address.sin_addr = (h->sin_addr);
  
  //Verbindung herstellen
  if( connect(socke, (struct sockaddr *) &address, sizeof(address)) != 0) {
    perror("Verbindung fehlgeschlagen!");
  }
  
  performConnection(socke, gameId);
  
}