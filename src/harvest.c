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
#include <string.h>        //memset
#include <ifaddrs.h>       //get_interfaces
#include <arpa/inet.h>     //get_interfaces
#include <sys/socket.h>    //get_interfaces
#include <netdb.h>         //get_interfaces
#include <linux/if_link.h> //get_interfaces
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

bool get_kernel(char *buffer)
{
    struct utsname utsbuf;

    memset(&utsbuf, 0, sizeof utsbuf);

    if (uname(&utsbuf) < 0)
        return false;

    sprintf(buffer + strlen(buffer), "Sysname:%s\r\n", utsbuf.sysname);
    sprintf(buffer + strlen(buffer), "Nodename:%s\r\n", utsbuf.nodename);
    sprintf(buffer + strlen(buffer), "Release:%s\r\n", utsbuf.release);
    sprintf(buffer + strlen(buffer), "Version:%s\r\n", utsbuf.version);
    sprintf(buffer + strlen(buffer), "Machine:%s\r\n", utsbuf.machine);

    return true;
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

/* Mainly from man getifaddrs */

bool get_interfaces()
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[1024];
    int max_host = 1024;

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return false;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;
        printf("%-8s %s (%d)\n",
               ifa->ifa_name,
               (family == AF_PACKET) ? "AF_PACKET" : (family == AF_INET) ? "AF_INET"
                                                 : (family == AF_INET6)  ? "AF_INET6"
                                                                         : "???",
               family);
        if (family == AF_INET || family == AF_INET6)
        {
            s = getnameinfo(ifa->ifa_addr,
                            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                            host, max_host,
                            NULL, 0, 1);
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return false;
            }

            printf("\t\taddress: <%s>\n", host);
        }
        else if (family == AF_PACKET && ifa->ifa_data != NULL)
        {
            struct rtnl_link_stats *stats = ifa->ifa_data;

            printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
                   "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
                   stats->tx_packets, stats->rx_packets,
                   stats->tx_bytes, stats->rx_bytes);
        }
    }

    /*
    Macs -> /sys/class/net/nombre interface/address
    */

    freeifaddrs(ifaddr);
    return true;
}
