#ifndef shm
#define shm 

#define ANZAHLSPIELER 2

struct playerAttr{
int spielerNr;
char name[256] ; // auf Größe achten
int registered; // 1, wenn registiert/bereit ist, 0, wenn nicht
};
 
struct shmInfos{
	char spielname[30]; // kann nur 30 Zeichen lang sein
	int eigSpielernummer;
	int anzahlSpieler;
	int pid0;
	int pid1;
	struct playerAttr attr[ANZAHLSPIELER];
};

//Shared Memory Segment erstellen 
int shmSegment();
//Shared Memory Bereich an die shmId anbinden
struct shmInfos* shmAnbinden(int shmid);
//Segment Loeschen bei Prozessende
void shmDelete(int shmid);

#endif

