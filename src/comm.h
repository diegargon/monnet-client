/**
 * 
 * 
 */ 

#ifndef COMM_H
#define COMM_H

char *build_msg(char *msg, char *payload);
bool send_msg(int socket_fd, char* msg);
int conn_send_msg(char *msg);
//bool check_msg(char *msg);
int server_connect();
bool server_disconnect(int socket_fd);

#endif
