/**
 * 
 * 
 */

#ifndef COMM_H
#define COMM_H

char *build_msg(struct MonnetHeader *mHeader, char *payload);
bool send_msg(int socket_fd, struct MonnetHeader **mHeader, char *payload);
bool receive_msg(int socket_fd, struct MonnetHeader **mHeader, char *payload);
int server_connect();
bool server_disconnect(int socket_fd);
char *get_hello_payload(void);

#endif
