/**
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>

void error_warning(char *msg)
{
    //TODO: if(daemon) { SYSLOG } else { printf }
    perror(msg);
}

void error_fatal(char *msg)
{
    //TODO: if(daemon) { SYSLOG } else { printf }
    perror(msg);
    exit(1);
}
