#ifndef network
#define network 

//Verbindung mit Server herstellen 
int netConnect(char* port, char* hostname);
//Verbindung zum Server trennen 
int netDisconnect();
//eine Zeile einlesen, die der Server sendet
char* netReadLine();
//dem Server eine Zeile ins Socket schreiben
int netWrite(char *bufferSend);
//socket fd auslesen
int getSocketFd();
#endif

