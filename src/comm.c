/**
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include "config.h"
#include "utils.h"
#include "comm.h"
#include "error.h"
#include "harvest.h"

char *build_msg(struct MonnetHeader *mheader, char *payload)
{
    char *header = (char *)malloc(sizeof(char) * 1024);

    sprintf(header, "Version:%d\r\n", VERSION);
    sprintf(header + strlen(header), "Auth:%s\r\n", CLIENT_AUTH);
    sprintf(header + strlen(header), "Ack:%d\r\n", mheader->ack);
    sprintf(header + strlen(header), "Msg:%s\r\n", mheader->msg);
    sprintf(header + strlen(header), "Size:%ld\r\n", mheader->size);
    sprintf(header + strlen(header), "\r\n\r\n");
    if (mheader->size > 0)
        sprintf(header + strlen(header), "%s", payload);

    return header;
}

char *get_hello_payload()
{
    char *buffer = (char *)malloc(sizeof(char) * 1024);

    if (get_kernel(buffer))
        return buffer;
    else
        return NULL;
}

int server_connect()
{
    int socket_fd = -1;
    int port = SERVER_PORT;
    char *server_ip = SERVER_IP;
    struct sockaddr_in address;
    struct timeval tm;

    tm.tv_sec = 10;
    tm.tv_usec = 0;

    //Create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error_warning("Server Connect: Socket creation error");
        return -1;
    }

    /* Fill IPv4 struct */

    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    /* Set ip */
    if ((inet_pton(AF_INET, server_ip, &address.sin_addr)) <= 0)
    {
        error_warning("Server Connect: Invalid address");
        return -1;
    }

    if ((setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct timeval))) < 0)
    {
        printf("Error setting recive timeout\n");
    }

    if ((setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(struct timeval))) < 0)
    {
        printf("Error setting send timeout\n");
    }

    if ((connect(socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        error_warning("Server Connect: Connection Failed");
        return -1;
    }

    return socket_fd;
}

bool server_disconnect(int socket_fd)
{
    printf("Discconect from server\n");
    close(socket_fd);
    return true;
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
        //sendBuffer = build_msg("ACK", NULL);
    }
    else
    {
        //Not valid head found
        //sendBuffer = build_msg("NACK", NULL);
    }

    printf("Response (%ld):\n%s", recive_total_bytes, payload);

    return true;
}
