#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sharedmemory.h"

//#define ANZAHLSPIELER 2

//struct playerAttr{
//	int spielerNr;
//	char name[256] ; // auf Größe achten
//	int registered; // 1, wenn registiert/bereit ist, 0, wenn nicht
//};
//
//struct shmInfos{
//char spielname[30]; // kann nur 30 Zeichen lang sein
//int eigSpielernummer;
//int anzahlSpieler;
//int pid1;
//int pid2;
//struct playerAttr attr[ANZAHLSPIELER];
//};

// Shared Memory Segment wird erstellt und der ID des gemeinsamen Speicherbereichs zugewiesen
// Groesse des SHM: struktur + Eigenschaften von jedem Spieler (wieviele Spieler?)
//erstmal fuer 2 Spieler (bis Loesung fuer shm-vergroesserung oder aehnliches
int shmSegment(){
	int shmid;
	if((shmid = shmget(IPC_PRIVATE, sizeof(struct shmInfos), IPC_CREAT | 0666) )==-1){
	perror("Fehler beim Segment erstellen");
	}
	return shmid;
}

//Shared-Memory-Bereich anbinden
//muss bei jedem Prozess eingefügt werden:

struct shmInfos* shmAnbinden(int shmid){
	//char *shm_ptr = shmat(shmid, NULL, 0); //attach
	//if (shm_ptr==(char *)-1) {
	struct shmInfos *shm_ptr = shmat(shmid, NULL, 0); //attach
	if (shm_ptr==(struct shmInfos*)-1) {
		perror("Fehler beim Binden des SHM an einen Prozess");
	}
	return shm_ptr;
}

// Segment wieder löschen, wenn sich der letzte Prozess detached (nach! dem ersten attach ausführen)
void shmDelete(int shmid){
	if(shmctl(shmid, IPC_RMID, NULL)==-1) {
	perror("Fehler beim Loeschen des Shared Memory Segments");
	}
}
