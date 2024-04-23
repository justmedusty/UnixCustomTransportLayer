
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

#ifndef UNIXCUSTOMTRANSPORTLAYER_DUSTYNS_TRANSPORT_LAYER_H
#define UNIXCUSTOMTRANSPORTLAYER_DUSTYNS_TRANSPORT_LAYER_H

#define BACKLOG 15
#define PACKET_SIZE 512
#define HEADER_SIZE 64
#define MAX_PACKET_COLLECTION 1000
#define ACKNOWLEDGE "ACK"
#define CORRUPTION "BAD_DATA"
#define RESEND "RESEND"
#define CLOSE "DISCONNECT"
#define TIMEOUT_SECONDS 10
#define SUCCESS 0
#define ERROR (-1)

void handle_client_connection(int socket);
int send_resend(int socket, uint16_t sequence);
typedef struct Packet {
    struct iphdr ip_header;
    struct iovec iov[2];
} Packet;

typedef struct Header {
    char status[20];
    uint16_t checksum;
    uint16_t sequence;

} Header;


#endif //UNIXCUSTOMTRANSPORTLAYER_DUSTYNS_TRANSPORT_LAYER_H
