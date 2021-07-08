/**
 * 
 * 
 */

#ifndef COMM_C_H
#define COMM_C_H

int server_connect();
bool server_disconnect(int socket_fd);
char *get_hello_payload(void);

#endif
