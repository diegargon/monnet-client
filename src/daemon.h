/**
 * 
 * 
 */

#ifndef DAEMON_H
#define DAEMON_H

void start_daemon(void (*function)());
void signal_handler(int signal_number);

#endif
