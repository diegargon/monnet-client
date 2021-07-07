/**
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h> // remove

#include <ctype.h> //isspace

#include "utils.h"

char *ltrim(char *s)
{
    while (isspace(*s))
        s++;
    return s;
}

char *rtrim(char *s)
{
    char *back = s + strlen(s);
    while (isspace(*--back))
        ;
    *(back + 1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s));
}

size_t get_payload_size(char *buf)
{
    size_t size = 0;
    char *value = {0};

    while (sscanf(buf, "Size=%s", value))
        ;
    size = (size_t)value;
    return size;
}

struct MonnetHeader get_header(char *header)
{
    struct MonnetHeader mheader;
    char key[31];
    char val[31];

    memset(&mheader, 0, sizeof mheader);

    char *line = strtok(strdup(header), "\r\n");

    while (line)
    {
        //if (sscanf(line, "%127[^:]:%127[^\r\n]%*c", key, val) == 2)
        if (sscanf(line, "%31[a-zA-Z_0-9]:%31s", key, val) == 2)
        {

            if (strcmp(key, "Version") == 0)
            {
                mheader.version = atoi(val);
            }
            if (strcmp(key, "Auth") == 0)
            {
                //mheader.auth = malloc(strlen(val)+1);
                strcpy(mheader.auth,val);
            }
            if (strcmp(key, "Size") == 0)
            {
                mheader.size = atoi(val);
            }

            if (strcmp(key, "Msg") == 0)
            {
                //mheader.msg = malloc(strlen(val)+1);
                strcpy(mheader.msg, val);
            }
        }
        line = strtok(NULL, "\r\n");

    }

    return mheader;
}
