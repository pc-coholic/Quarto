#include "pipe.h"

// Ueberwacht ob Server oder Pipe etwas geschrieben hat 
// Wenn in die Pipe geschrieben wurde dann gibt die Funktion 1 zurueck, 
// wenn vom Server Timeout gesendet wird 2,  sonst 0

extern char spielzug[10];
// Liest aus der Pipe[
int pipe_read(int pipe_fd[],char * getText){
	close(pipe_fd[WRITE]);	
	if((read(pipe_fd[READ],getText,10)) < 0) {
		log_printf(LOG_ERROR,"Fehler beim Lesen der Pipe");
		perror("Error: ");
		return 0;
	}
	
	log_printf(LOG_DEBUG,"Pipe gelesen: %s\n", getText);
	return 1;
	
	
}
// Schreibt in die Pipe 
int pipe_write(int pipe_fd[]){
	close(pipe_fd[READ]);
	// In die Pipe schreiben
	if((write(pipe_fd[WRITE],spielzug,6))!=6){
		log_printf(LOG_ERROR,"Fehler beim Schreiben in die Pipe\n");	
		return 0;
	}
	//close(pipe_fd[WRITE]);
	log_printf(LOG_DEBUG,"Pipe geschrieben\n");
	return 1;
}

/* return Wert:
   -1,0 : Fehler
   1 : Pipe ist ready
   2 : Socket lesbar
*/
int ueberwacheFd(int pipe_fd[],char *getText){
	int socket_fd = getSocketFd();
	fd_set set;
	FD_ZERO(&set);
	FD_SET(pipe_fd[READ],&set);
	FD_SET(socket_fd,&set);
	int max = pipe_fd[READ];
	if (max < socket_fd) {
		max = socket_fd;
	}
	int ret = select(max+1,&set,NULL,NULL,NULL);
	
	if(ret < 1){
		log_printf(LOG_ERROR,"Fehler bei Select\n");
		perror("error");
		return 0;
	}
	else if (FD_ISSET(pipe_fd[READ],&set)) {
			log_printf(LOG_DEBUG,"Gandalf hat gedacht.\n");
			return pipe_read(pipe_fd, getText);		}
	log_printf(LOG_DEBUG,"Gandalf denkt zu langsam.\n");
	return 2;
}
	
