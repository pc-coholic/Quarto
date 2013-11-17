#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct infos{
char[] spielname;
int spielernummer;
int anzahlSpieler;
int pid1;
int pid2;
};

// In der Angabe steht keine MagicNumbers also einfach 0 als keyval? Ansonsten:
// keyval muss erzeugt werden oder kann als Konstante definiert werden
key_t key1 = ftok("/usr/include", 'x'); //weiss nicht welchen pathname man angeben soll

// key_t ftok(const char *pathname, int proj_id);


// Shared Memory Segment wird erstellt und der ID des gemeinsamen Speicherbereichs zugewiesen

if((int shmid = shmget(key1, sizeof(struct infos), IPC_PRIVATE))==-1){
perror("Fehler beim Segment erstellen");
}

//.... was gemacht werden soll dazwischen


// Shared Memory Segment anbinden
if((int adrAnfang = void *shmat(shmid,NULL,0))==-1){
perror("Fehler beim Binden an einen Prozess");
}





// Segment wieder löschen bei Prozessende
if((int loeschen = shmctl(shmid,IPC_RMID,shmid_ds))==-1){
perror("Fehler beim Loeschen des Shared Memory Segments");
}
