/**
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "monnet-client.h"
#include "harvest.h"

void monnet_client()
{
    int loadavg = 0;
    time_t last_time = time(NULL);
    int update_cycle = 5;

    while (1)
    {
        if ((last_time + update_cycle) <= time(NULL))
        {
            loadavg = get_loadavg();
            printf("Time: %lu / %ld\n", time(NULL), (last_time + update_cycle));
            printf("Load avg: %d%%\n", loadavg);
            //free(loadavg);
            last_time = time(NULL);
            get_kernel();
            char *distro_name = get_distro();
            if (distro_name != NULL)
            {
            }
            printf("Distro %s\n", distro_name);
        }
        sleep(1);
    }
}