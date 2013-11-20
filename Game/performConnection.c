#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CLIENTVERSION 1.0
#define BUF 251
//Funktion prueft, ob buffer vom Server mit '-' beginnt (wenn ja, wird abgebrochen)
void checkMinus(char *buffer) {
	if(buffer[0] == '-') {
		perror("Verbindung wurde nicht akzeptiert");
		exit(EXIT_FAILURE);
	}
}

//Methode gibt bei Fehler 0 zurueck, bei Erfolg 1
int performConnection(int socket, char* gameId, int player) {
	char bufferSend[BUF]; 
	char bufferGet[BUF]; 
	int len = 0;  

  //get gameserver version
	len = recv(socket, bufferGet, sizeof(bufferGet),0);
	bufferGet[len]='\0';
  printf("S: %s",bufferGet);
  //checkMinus(buffer);
  

  //Client-Version an Server senden
	snprintf(bufferSend,13,"VERSION %f",CLIENTVERSION);
	len = strlen(bufferSend);
	bufferSend[len] = '\0';
	bufferSend[len+1] = '\n';
	if(write(socket, bufferSend, (len+2)*sizeof(char)) < 0) {
		perror("Fehler beim Schreiben in den Socket");
	}
   printf("C: %s\n",bufferSend);
   

  //Client-Version vom Server akzeptiert?
	len = recv(socket, bufferGet, sizeof(bufferGet),0);
	bufferGet[len]='\0';
	printf("S: %s",bufferGet);
  //checkMinus(buffer);
   

  //Game_ID an Server senden 
	snprintf(bufferSend,16,"ID %s",gameId);
	len = strlen(bufferSend);
	bufferSend[len] = '\0';
	bufferSend[len+1] = '\n';
	if(write(socket, bufferSend, (len+2)*sizeof(char)) < 0) {
		perror("Fehler beim Schreiben in den Socket");
	}
	printf("C: %s\n",bufferSend);
   
	//Server: Welches Spiel?
	len = recv(socket, bufferGet, sizeof(bufferGet),0);
	bufferGet[len]='\0';
	printf("S: %s",bufferGet);
	//checkMinus(buffer);

	// Fehlermeldung und Beenden vom Client falls Spiel != Quarto 
	if(strcmp((bufferGet+10),"Quarto")==0) {
		perror("Du spielst nicht Quarto, du Depp!");
		exit(EXIT_FAILURE);
	}	

	//Server: Game-Name 
	len = recv(socket, bufferGet, sizeof(bufferGet),0);
	bufferGet[len]='\0';
	printf("S: %s",bufferGet);
	//checkMinus(buffer);

	// Gewuenschte Spielernummer an Server senden 
	// optional Spielernummer angeben
	if (player!='3') {
		snprintf(bufferSend,7,"PLAYER %c", player);
	}
	else {
		snprintf(bufferSend,7,"PLAYER");
	}
	len = strlen(bufferSend);
	bufferSend[len] = '\0';
	bufferSend[len+1] = '\n';
	if(write(socket, bufferSend, (len+2)*sizeof(char)) < 0) {
		perror("Fehler beim Schreiben in den Socket");
	}
	printf("C: %s\n",bufferSend);

	//Server: zugeteilte Spielernummer und Name 
	len = recv(socket, bufferGet, sizeof(bufferGet),0);
	bufferGet[len]='\0';
	printf("S: %s",bufferGet);
	//checkMinus(buffer);

	//Server: SPieleranzahl 
	len = recv(socket, bufferGet, sizeof(bufferGet),0);
	bufferGet[len]='\0';
	printf("S: %s",bufferGet);

	//Server: Endplayers 
	len = recv(socket, bufferGet, sizeof(bufferGet),0);
	bufferGet[len]='\0';
	printf("S: %s",bufferGet);

	return 0;
}
