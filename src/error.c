/**
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>

void error_warning(char *msg)
{
    perror(msg);
}

void error_fatal(char *msg)
{
    perror(msg);
    exit(1);
}
