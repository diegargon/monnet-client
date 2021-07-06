/**
 * 
 * 
 */

#include <signal.h>
#include <stdlib.h>
#include <syslog.h>

void signal_handler(int signal_number) {    
    syslog(LOG_WARNING, "Received signal.");
    exit(0);
}


void start_daemon(void (*client_main)()) {
    /* TODO Become daemon*/
        /* Signals */

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGINT, signal_handler);

    client_main();   
}