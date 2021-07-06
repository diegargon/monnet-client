/**
 * 
 * 
 */

#include "monnet-client.h"
#include "main.h"
#include "daemon.h"

int main(int argc, char *argv[])
{

    void (*client_main)() = monnet_client;
    start_daemon(client_main);
}
