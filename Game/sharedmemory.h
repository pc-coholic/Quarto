#ifndef shm
#define shm 

//Shared Memory Segment erstellen 
int shmSegment();
//Shared Memory Bereich an die shmId anbinden
void shmAnbinden(int shmid);
//Segment Loeschen bei Prozessende
void shmDelete(int shmid);

#endif

