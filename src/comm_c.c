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
#include <unistd.h>
#include "config.h"
#include "error.h"
#include "harvest.h"

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