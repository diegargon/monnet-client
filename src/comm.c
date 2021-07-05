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

#include "config.h"
#include "comm.h"
#include "error.h"

char *get_header(char *msg, char *payload)
{

    char *header = (char *)malloc(sizeof(char) * 1024);

    sprintf(header, "Version:%d\r\n", 3);

    sprintf(header + strlen(header), "Auth:%s\r\n", CLIENT_AUTH);
    sprintf(header + strlen(header), "Msg:%s\r\n", msg);

    if (strcmp(msg, "POST_JSON") && payload)
    {
        sprintf(header + strlen(header), "Size:%ld\r\n", strlen(payload));
        sprintf(header + strlen(header), "\r\n\r\n");
        sprintf(header + strlen(header), "%s", payload);
    }
    else
    {
        sprintf(header + strlen(header), "\r\n\r\n");
    }

    return header;
}

int send_msg(char *msg) {
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

    //set_header(header,"hello", NULL);
    //header = get_header("hello", NULL);
        
    send(socket_fd, msg , strlen(msg), 0);
    //free(msg);
    read_bytes = read(socket_fd, read_buffer, 1024);
    printf("%s (%d)\n", read_buffer, read_bytes);

    close(socket_fd);

    return 0;
}