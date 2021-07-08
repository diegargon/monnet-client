/**
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "config.h"
#include "utils.h"
#include "comm.h"

char *build_msg(struct MonnetHeader *mheader, char *payload)
{
    char *header = (char *)malloc(sizeof(char) * 1024);

    sprintf(header, "Version:%d\r\n", VERSION);
    sprintf(header + strlen(header), "Auth:%s\r\n", mheader->auth);
    sprintf(header + strlen(header), "Ack:%d\r\n", mheader->ack);
    sprintf(header + strlen(header), "Msg:%s\r\n", mheader->msg);
    sprintf(header + strlen(header), "Size:%ld\r\n", mheader->size);
    sprintf(header + strlen(header), "\r\n\r\n");
    if (mheader->size > 0)
        sprintf(header + strlen(header), "%s", payload);

    return header;
}

bool send_msg(int socket_fd, struct MonnetHeader **mHeader, char *payload)
{
    char *msg = {0};
    int sended_bytes = 0;
    size_t sended_total_bytes = 0;

    msg = build_msg((*mHeader), payload);
    printf("Sending msg:");
    while (sended_total_bytes < strlen(msg))
    {
        if ((sended_bytes = send(socket_fd, msg, strlen(msg), 0)) == -1)
        {
            perror("error");
            printf("[Fail]\n");
            return false;
        }
        sended_total_bytes += sended_bytes;
    }
    printf("[OK]\n");

    //Send ask for ACK
    if ((*mHeader)->ack == 1)
    {
        char *response_payload = {0};
        struct MonnetHeader *response_head = malloc(sizeof(struct MonnetHeader));

        if (receive_msg(socket_fd, &response_head, response_payload))
        {
            if (strcmp(response_head->msg, "ACK") == 0)
            {
                printf("Got ACK response\n");
                free(response_head);
                return true;
            }
        }
        else
        {
            free(response_head);
            return false;
        }
    }

    return true;
}

bool receive_msg(int socket_fd, struct MonnetHeader **mHeader, char *payload)
{
    char read_buffer[8192] = {0};
    int read_bytes = 0;
    size_t recive_total_bytes = 0;
    size_t recive_size = 8192;
    char *end_head = NULL;
    int paytotal = 0;

    printf("Reading msg: \n");
    while (recive_total_bytes < recive_size)
    {
        read_bytes = recv(socket_fd, read_buffer, sizeof(read_buffer), 0);
        recive_total_bytes += read_bytes;
        if (read_bytes < 0)
        {
            printf("Error reading header\n");
            return false;
        }

        if ((end_head = strstr(read_buffer, END_HEAD)) != NULL)
        {
            printf("Found Valid header\n");
            break;
        }
        if (read_bytes == 0)
        {
            printf("Error: Valid Head not detected\n");
            return false;
        }
    }
    if (end_head != NULL)
    {
        printf("End head (%ld) *%s*\n", strlen(end_head), end_head);
        size_t recv_extra = strlen(end_head) - strlen(END_HEAD) - 1;
        //printf("ENDHEAD (%ld) -> *%s*\n", strlen(end_head) - strlen(END_HEAD) -2, end_head+strlen(END_HEAD)+2);

        *mHeader = get_header(read_buffer);
        //Get rest if any payload
        if (recv_extra < (*mHeader)->size)
        {
            printf("Head:OK: Payload: PARTIALLY (R:%ld/P:%ld)\n", recv_extra, (*mHeader)->size);
            //rid head
            strcpy(read_buffer, (end_head + strlen(END_HEAD) + 2));

            paytotal = strlen(read_buffer);
            printf("Paytotal %d / Pay %ld\n", paytotal, (*mHeader)->size);

            while (paytotal < (*mHeader)->size)
            {
                //TODO check if mheader.size is bigger than RECV_BUFFER
                read_bytes = recv(socket_fd, &read_buffer[paytotal], (*mHeader)->size, 0);
                if (read_bytes < 0)
                {
                    printf("Error reciving \n");
                    break;
                }
                if (read_bytes == 0)
                {
                    break;
                }
                paytotal += read_bytes;
            }
        }
        else
        {
            printf("Head:OK Payload: OK (R:%ld/P:%ld)\n", recv_extra, (*mHeader)->size);
            printf("Msg-> %s\n", (*mHeader)->msg);
            payload = (char *)malloc(sizeof(char) * strlen(end_head));
            //rid head
            strcpy(payload, (end_head + strlen(END_HEAD) + 2));
        }
        //Send ACK if sender want
        if ((*mHeader)->ack == 1)
        {
            struct MonnetHeader *reply_head = malloc(sizeof(struct MonnetHeader));
            reply_head->size = 0;
            reply_head->ack = 0;
            strcpy(reply_head->auth, (*mHeader)->auth);
            strcpy(reply_head->msg, "ACK");
            send_msg(socket_fd, &reply_head, NULL);
            free(reply_head);
        }        
    }
    else
    {
        //Not valid head found
        return false;
    }

    printf("Response (%ld):\n%s", recive_total_bytes, payload);

    return true;
}


struct MonnetHeader *get_header(char *header)
{
    struct MonnetHeader *mheader = malloc(sizeof(struct MonnetHeader));
    char key[31];
    char val[31];
    char *line = strtok(strdup(header), "\r\n");

    while (line)
    {
        if (sscanf(line, "%31[a-zA-Z_0-9]:%31s", key, val) == 2)
        {

            if (strcmp(key, "Version") == 0)
            {
                mheader->version = atoi(val);
            }
            if (strcmp(key, "Auth") == 0)
            {
                //mheader.auth = malloc(strlen(val)+1);
                strcpy(mheader->auth, val);
            }
            if (strcmp(key, "Size") == 0)
            {
                mheader->size = atoi(val);
            }
            if (strcmp(key, "Ack") == 0)
            {
                mheader->ack = atoi(val);
            }
            if (strcmp(key, "Msg") == 0)
            {
                //mheader.msg = malloc(strlen(val)+1);
                strcpy(mheader->msg, val);
            }
        }
        line = strtok(NULL, "\r\n");
    }

    return mheader;
}
