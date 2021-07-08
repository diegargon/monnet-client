/**
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "monnet-client.h"
#include "utils.h"
#include "comm.h"
#include "comm_c.h"
#include "harvest.h"

void monnet_client()
{
    time_t last_time = 0;
    int update_cycle = 5;
    char *msg = {0};
    int socket_fd = -1;
    int hello = 0;
    char *payload;

    while (1)
    {
        if ((last_time + update_cycle) <= time(NULL))
        {
            if (hello == 0)
            {
                if ((socket_fd = server_connect()) > 0)
                {
                    //struct MonnetHeader *hello_head = NULL;
                    struct MonnetHeader *hello_head = malloc(sizeof(struct MonnetHeader));
                    payload = get_hello_payload();
                    hello_head->size = strlen(payload);
                    hello_head->ack = 1;
                    strcpy(hello_head->msg, "HELLO");
                    if (send_msg(socket_fd, &hello_head, payload))
                    {
                        printf("Msg send OK\n");
                        hello = 1;
                    }
                    else
                    {
                        printf("Msg Send FAIL\n");
                    }

                    free(msg);
                    free(payload);
                    free(hello_head);

                    server_disconnect(socket_fd);
                }
            }
            else
            {
                //Hello ok
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

        sleep(5);
    }
}
