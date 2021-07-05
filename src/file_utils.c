/**
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "utils.h"

bool file_exists(char *path)
{
    if (access(path, F_OK) == 0)
        return true;
    return false;
}

bool file_access(char *path, char *perms)
{
    if (!file_exists(path))
    {
        return false;
    }
    int mask = 0;

    if (strcmp(perms, "rw") == 0 || strcmp(perms, "w") == 0)
    {
        mask = W_OK;
    }
    else if (strcmp(perms, "r") == 0)
    {
        mask = R_OK;
    }
    else
    {
        return false;
    }

    if (access(path, mask) == 0)
        return true;

    return false;
}

char *fread_key(FILE *fp, char const *keyname)
{
    char key[128];
    char val[128];

    while (fscanf(fp, "%127[^=]=%127[^\n]%*c", key, val) == 2)
    {

        /* printf("Key %s Value %s\n", key, val); */
        if (strcmp(key, keyname) == 0)
        {
            size_t size_val = strlen(val);
            /* Remove "" from string if exists */
            if (val[0] == '"')
            {
                memmove(val, val + 1, strlen(val) - 2);
                val[size_val - 2] = 0;
            }
            //TODO TRIM
            return strdup(val);
        }
    }
    return NULL;
}
