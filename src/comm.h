/**
 * 
 * 
 */

#ifndef COMM_H
#define COMM_H

struct MonnetHeader
{
    int version;
    int ack;
    char auth[32];
    char msg[32];
    size_t size;
};

char *build_msg(struct MonnetHeader *mHeader, char *payload);
bool send_msg(int socket_fd, struct MonnetHeader **mHeader, char *payload);
bool receive_msg(int socket_fd, struct MonnetHeader **mHeader, char **payload);

struct MonnetHeader *get_header(char *header);

#endif
