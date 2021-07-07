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
#include "comm.h"
#include "error.h"
#include "utils.h"
#include "harvest.h"

char *build_msg(char *msg, int ack, char *payload)
{

    char *header = (char *)malloc(sizeof(char) * 1024);
    size_t payload_size = 0;
    if (payload != NULL)
        payload_size = strlen(payload);

    sprintf(header, "Version:%d\r\n", VERSION);
    sprintf(header + strlen(header), "Auth:%s\r\n", CLIENT_AUTH);
    sprintf(header + strlen(header), "Ack:%d\r\n", ack);
    sprintf(header + strlen(header), "Msg:%s\r\n", msg);
    sprintf(header + strlen(header), "Size:%ld\r\n", payload_size);
    sprintf(header + strlen(header), "\r\n\r\n");
    if (payload_size > 0)
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

bool send_msg(int socket_fd, char *msg)
{

    int sended_bytes = 0;
    size_t sended_total_bytes = 0;

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

    return true;
}

bool receive_msg(int socket_fd)
{
    char read_buffer[8192] = {0};
    int read_bytes = 0;
    size_t recive_total_bytes = 0;
    size_t recive_size = 8192;
    char *end_head = NULL;
    int paytotal = 0;

    struct MonnetHeader mheader;

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
        mheader = get_header(read_buffer);
        if (recv_extra < mheader.size)
        {
            printf("Head:OK: Payload: PARTIALLY (R:%ld/P:%ld)\n", recv_extra, mheader.size);
            //rid head
            strcpy(read_buffer, (end_head + strlen(END_HEAD) + 2));

            paytotal = strlen(read_buffer);
            printf("Paytotal %d / Pay %ld\n", paytotal, mheader.size);

            while (paytotal < mheader.size)
            {
                //TODO check if mheader.size is bigger than RECV_BUFFER
                read_bytes = recv(socket_fd, &read_buffer[paytotal], mheader.size, 0);
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
            printf("Head:OK Payload: OK (R:%ld/P:%ld)\n", recv_extra, mheader.size);
            //rid head
            strcpy(read_buffer, (end_head + strlen(END_HEAD) + 2));
        }
        //sendBuffer = build_msg("ACK", NULL);
    }
    else
    {
        //Not valid head found
        //sendBuffer = build_msg("NACK", NULL);
    }

    printf("Response (%ld):\n%s", recive_total_bytes, read_buffer);

    return true;
}

int conn_send_msg(char *msg)
{
    int socket_fd = 0;
    int read_bytes;
    int port = SERVER_PORT;
    char *server_ip = SERVER_IP;
    char read_buffer[1024] = {0};
    struct sockaddr_in address;

    //Create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error_fatal("Socket creation error");
    }

    /* Fill IPv4 struct */

    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    /* Set ip */
    if (inet_pton(AF_INET, server_ip, &address.sin_addr) <= 0)
    {
        error_fatal("Invalid address");
    }

    if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        error_fatal("Connection Failed");
    }

    send(socket_fd, msg, strlen(msg), 0);
    //free(msg);
    read_bytes = read(socket_fd, read_buffer, 1024);
    printf("%s (%d)\n", read_buffer, read_bytes);

    close(socket_fd);

    return 0;
}
