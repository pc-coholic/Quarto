#ifndef cnfig
#define cnfig 

#define MAXBUF 1024

struct config
{
   char hostname[MAXBUF];
   char port[MAXBUF];
   char gametype[MAXBUF];
   int loglevel;
};


struct config configstruct;
struct config get_config(char *filename);

#endif
