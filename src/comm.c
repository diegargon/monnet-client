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

char *build_msg(char *msg, char *payload)
{

    char *header = (char *)malloc(sizeof(char) * 1024);
    size_t payload_size = 0;
    if (payload != NULL)
        payload_size = strlen(payload);

    sprintf(header, "Version:%d\r\n", VERSION);
    sprintf(header + strlen(header), "Auth:%s\r\n", CLIENT_AUTH);
    sprintf(header + strlen(header), "Msg:%s\r\n", msg);
    sprintf(header + strlen(header), "Size:%ld\r\n", payload_size);
    sprintf(header + strlen(header), "\r\n\r\n");
    if (payload_size > 0)
        sprintf(header + strlen(header), "%s", payload);

    return header;
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

    char read_buffer[8096] = {0};
    size_t sended_bytes = 0;
    size_t sended_total_bytes = 0;
    size_t read_bytes = 0;
    size_t recive_total_bytes = 0;
    size_t recive_size = 4096;
    char *end_head = NULL;

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

    //if (( read_bytes = recv(socket_fd, read_buffer, sizeof(read_buffer), 0)) <= 0)
    //   return false;

    printf("Waiting response: \n");
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
    printf("Response (%ld):\n%s", recive_total_bytes,  read_buffer);
    return true;
}

int receive_msg(int socket_fd, char *msg)
{
    /*
        Lee linea por linea \r\n y trata los valores de la header
        Si encuentra una linea en blanco termino la header
    */
    return 0;
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

    //set_header(header,"hello", NULL);
    //header = get_header("hello", NULL);
    send(socket_fd, msg, strlen(msg), 0);
    //free(msg);
    read_bytes = read(socket_fd, read_buffer, 1024);
    printf("%s (%d)\n", read_buffer, read_bytes);

    close(socket_fd);

    return 0;
}
