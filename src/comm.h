/**
 * 
 * 
 */ 

#ifndef COMM_H
#define COMM_H

char *build_msg(char *msg, int ack, char *payload);
bool send_msg(int socket_fd, char* msg);
bool receive_msg(int socket_fd);

int conn_send_msg(char *msg);
//bool check_msg(char *msg);
int server_connect();
bool server_disconnect(int socket_fd);
char *get_hello_payload(void);

#endif
