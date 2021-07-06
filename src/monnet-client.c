/**
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include "monnet-client.h"
#include "comm.h"
#include "harvest.h"

void monnet_client()
{
    //int loadavg = 0;
    time_t last_time = 0;
    int update_cycle = 5;
    char *msg = {0};
    int socket_fd = -1;
    int hello = 0;

    while (1)
    {
        if ((last_time + update_cycle) <= time(NULL))
        {
            if (hello == 0)
            {
                if ( (socket_fd = server_connect()) > 0)
                {
                    msg = build_msg("HELLO", NULL);
                    if (send_msg(socket_fd, msg))
                        hello = 1;
                    server_disconnect(socket_fd);
                }
            } else  {
                printf("*");
                fflush(stdout);
            }


            /*
            msg = build_msg("HELLO", NULL);
            send_msg(msg);
            free(msg);
            */

            last_time = time(NULL);
        }
        /*
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
        */

        sleep(1);
    }
}
