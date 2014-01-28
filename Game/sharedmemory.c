#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sharedmemory.h"
#include "logger.h"

// Shared Memory Segment wird erstellt und der ID des gemeinsamen Speicherbereichs zugewiesen
// Groesse des SHM: struktur + Eigenschaften von jedem Spieler 
int shmSegment(int size){
	int shmid;
	if((shmid = shmget(IPC_PRIVATE, sizeof(size), IPC_CREAT | 0666) )==-1){
		perror("Fehler beim Segment erstellen");
	}
	return shmid;
}

//Shared-Memory-Bereich anbinden
//muss bei jedem Prozess eingefügt werden (bzw. vor fork)
struct shmInfos* shmAnbinden(int shmid){
	struct shmInfos *shm_ptr = shmat(shmid, NULL, 0); //attach
	if (shm_ptr==(struct shmInfos*)-1) {
		perror("Fehler beim Binden des SHM an einen Prozess");
	}
	return shm_ptr;
}

int* shmSpielfeldAnbinden(int shmid){
	int *shm_ptr_Sf = shmat(shmid, NULL, 0); //attach
	if (shm_ptr_Sf==(int*)-1) {
		perror("Fehler beim Binden des SHM an einen Prozess");
	}
	return shm_ptr_Sf;
}

// Segment wieder löschen, wenn sich der letzte Prozess detached (NACH dem ersten attach ausführen)
void shmDelete(int shmid){
	if(shmctl(shmid, IPC_RMID, NULL)==-1) {
	perror("Fehler beim Loeschen des Shared Memory Segments");
	}
}
