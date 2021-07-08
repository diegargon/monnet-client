/**
 * 
 * 
 */ 

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "signals.h"

void signal_handler(int signal_number) {    
    syslog(LOG_WARNING, "Received signal.");
    exit(0);    
}

#endif
