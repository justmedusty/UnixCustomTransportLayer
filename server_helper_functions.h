//
// Created by dustyn on 4/22/24.
//
#include <netdb.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include "sys/socket.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "poll.h"
#include "netinet/in.h"
#include "errno.h"
#include "string.h"
#include "netinet/ip.h"
#ifndef UNIXCUSTOMTRANSPORTLAYER_SERVER_HELPER_FUNCTIONS_H
#define UNIXCUSTOMTRANSPORTLAYER_SERVER_HELPER_FUNCTIONS_H

#define BACKLOG 15
#define PORT "6969"

void *get_internet_addresses(struct sockaddr *sock_address);
int get_listener();
void signal_child_handler(int socket);

#endif //UNIXCUSTOMTRANSPORTLAYER_SERVER_HELPER_FUNCTIONS_H
