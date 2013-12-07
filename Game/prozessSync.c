#include "prozessSync.h"

//typedef void (*sighandler_t)(int);
//static volatile sig_atomic_t flag = 0;
static sigset_t mask_neu, mask_alt, mask_null;
static void sig_func(int);
static sighandler_t
my_signal (int sig_nr, sighandler_t signalhandler) {
   struct sigaction neu_sig, alt_sig;
   neu_sig.sa_handler = signalhandler;
   sigemptyset (&neu_sig.sa_mask);
   neu_sig.sa_flags = SA_RESTART;
   if (sigaction (sig_nr, &neu_sig, &alt_sig) < 0)
      return SIG_ERR;
   return alt_sig.sa_handler;
}
void start_sync(void) {
   my_signal( SIGUSR1, sig_func );
   my_signal( SIGUSR2, sig_func );
   sigemptyset(&mask_null);
   sigemptyset(&mask_neu);
   sigaddset(&mask_neu, SIGUSR1 );
   sigaddset(&mask_neu, SIGUSR2 );
   if( sigprocmask(SIG_BLOCK, &mask_neu, &mask_alt) < 0 ) {
      log_printf(LOG_ERROR,"Fehler bei sigprocmask() ...\n");
      exit(EXIT_FAILURE);
   }
}
static void sig_func( int sig_nr ) {
   start_sync();
   flag = 1;
}
void connector2thinker( pid_t pid ) {
   kill( pid, SIGUSR1 );
}
void thinker2connector( pid_t pid ) {
   kill( pid, SIGUSR2 );
}
void connector_waits( void) {
   while( !flag )
      sigsuspend(&mask_null);
   flag = 0;
   if( sigprocmask(SIG_BLOCK, &mask_alt, NULL) < 0 ) {
      log_printf(LOG_ERROR,"Fehler bei sigprocmask() ...\n");
      exit(EXIT_FAILURE);
   }
}
void thinker_waits(void ) {
   while( !flag )
      sigsuspend(&mask_null);
   flag = 0;
   if( sigprocmask(SIG_BLOCK, &mask_alt, NULL) < 0 ) {
      log_printf(LOG_ERROR,"Fehler bei sigprocmask() ...\n");
      exit(EXIT_FAILURE);
   }
}

void pidNull () {
int i;
      for(i = 0; i < MAX; i++) {
         connector_waits();
         log_printf(LOG_PRINTF,"Kind \n");
         connector2thinker(getppid());
       }
      log_printf(LOG_PRINTF,"--- Kind Ende ---\n");
      exit(EXIT_SUCCESS);
   }
void pidelse(int pid) {
int j;
   for(j = 0; j < MAX; j++) {
      log_printf(LOG_PRINTF,"Eltern \n");
      thinker2connector(pid);
      thinker_waits();
     }
   log_printf(LOG_PRINTF,"--- Eltern Ende ---\n");
}
