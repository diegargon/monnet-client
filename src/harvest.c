/**
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h> //uname
#include <sys/sysinfo.h>
#include <stdbool.h>
#include <string.h> //memset
#include "file_utils.h"

struct sysinfo get_sysinfo()
{
    /*
    unsigned long loads[3];  // 1, 5, and 15 minute load averages
    unsigned long totalram;  // Total usable main memory size
    unsigned long freeram;   // Available memory size
    unsigned long sharedram; // Amount of shared memory
    unsigned long bufferram; // Memory used by buffers
    unsigned long totalswap; // Total swap space size 
    unsigned long freeswap;  // Swap space still available 
    unsigned short procs;    // Number of current processes 
    unsigned long totalhigh; // Total high memory size 
    unsigned long freehigh;  // Available high memory size
    unsigned int mem_unit;   // Memory unit size in bytes 
    */
    struct sysinfo info;

    memset(&info, 0, sizeof info);

    if (sysinfo(&info) < 0)
    {
        //  return;
    }

    return info;
}

float get_loadavg()
{
    /*
    FILE *fp;
    //    char *buffer = (char * ) malloc(sizeof(char) * 10);
    char buffer[10];
    float loadavg = 0.0;

    fp = fopen("/proc/loadavg", "r");
    fscanf(fp, "%s", buffer);
    fclose(fp);
    loadavg = atof(buffer);

    return loadavg;
    */
    struct sysinfo info;
    float f_load = 1.f / (1 << SI_LOAD_SHIFT);

    printf("Procs %d \n", get_nprocs());
    info = get_sysinfo();
    return (int)info.loads[0] * f_load * 100 / get_nprocs();
}

void get_kernel()
{
    struct utsname buf;

    memset(&buf, 0, sizeof buf);

    if (uname(&buf) < 0)
    {
        //return;
    }
    printf("Sysname: %s \n", buf.sysname);
    printf("Nodename: %s \n", buf.nodename);
    printf("Release: %s \n", buf.release);
    printf("Version: %s \n", buf.version);
    printf("Machine: %s \n", buf.machine);
}

char *get_distro()
{
    char *str;
    FILE *fp;

    char file_path[] = "/usr/lib/os-release";

    if (file_access(file_path, "r"))
    {
        fp = fopen(file_path, "r");
        if (fp == NULL)
            return NULL;

        str = fread_key(fp, "NAME");

        if (str == NULL)
        {
            free(str);
            return NULL;
        }

        fclose(fp);
        return str;
    }

    return NULL;
}