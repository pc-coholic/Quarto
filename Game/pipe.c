#include "pipe.h"

// Ueberwacht ob Server oder Pipe etwas geschrieben hat 
// Wenn in die Pipe geschrieben wurde dann gibt die Funktion 1 zurueck, 
// wenn vom Server Timeout gesendet wird 2,  sonst 0

// Liest aus der Pipe
int pipe_read(int pipe_fd[]){
	close(pipe_fd[WRITE]);	
	char getText[6];
	read(pipe_fd[READ],getText,6);
	close(pipe_fd[READ]);
	if(strcmp(getText,"okay")!=0){
		log_printf(LOG_ERROR,"Fehler bei pipe_read\n");
		return 0;
	}
	return 1;
	
	
}
// Schreibt in die Pipe 
int pipe_write(int pipe_fd[]){
	close(pipe_fd[READ]);
	// In die Pipe schreiben
	if((write(pipe_fd[WRITE],"okay",5))!=5){
		log_printf(LOG_ERROR,"Fehler beim Schreiben in die Pipe\n");	
		return 0;
	}
	close(pipe_fd[WRITE]);
	
	return 1;
}

int ueberwacheFd(int pipe_fd[]){
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
	printf("Select ausgefuehrt\n");
	
	if (ret == 1) {
		if (FD_ISSET(pipe_fd[READ],&set)) {
			return pipe_read(pipe_fd);
		}
	}
		else if(ret < 1){
			log_printf(LOG_ERROR,"Fehler bei Select\n");
			return 0;
		}
		return 2;
	}
	





	