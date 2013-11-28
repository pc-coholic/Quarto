#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct shmInfos{
char[256] spielname; //wegen Länge überprüfen
int spielernummer;
int anzahlSpieler;
int pid1;
int pid2;
};


struct playerAttr{
	int spielerNr;
	char[256] name; // auf Größe achten
	int registered; // 1, wenn registiert/bereit ist, 0, wenn nicht
}


// Shared Memory Segment wird erstellt und der ID des gemeinsamen Speicherbereichs zugewiesen
// Groesse des SHM: struktur + Eigenschaften von jedem Spieler (wieviele Spieler?)
//erstmal fuer 2 Spieler (bis Loesung fuer shm-vergroesserung oder aehnliches
if((int shmid = shmget(IPC_PRIVATE, sizeof(struct shmInfos) + 2*sizeof(struct playerAttr), IPC_PRIVATE))==-1){
perror("Fehler beim Segment erstellen");
}

//Shared-Memory-Bereich anbinden
//muss bei jedem Prozess eingefügt werden:
if((char *anfAddr = smat(shmid, NULL, 0)) == -1) {
perror("Fehler beim Binden an einen Prozess");
}


// Segment wieder löschen bei Prozessende
if(shmctl(shmid, IPC_RMID, NULL)==-1) {
	perror("Fehler beim Loeschen des Shared Memory Segments");
}
