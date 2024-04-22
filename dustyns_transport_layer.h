
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
#define PACKET_SIZE 1024
#define HEADER_SIZE 256
#define ACKNOWLEDGE "ACK"
#define CORRUPTION "BAD_DATA"
#define CLOSE "DISCONNECT"
#define TIMEOUT_MICROSECONDS 5000000 // 5 seconds
#define CHECKSUM_GOOD 0
#define CHECKSUM_NOT_GOOD (-1)

void handle_client_connection(int socket);

typedef struct Packet {
    struct iphdr ip_header;
    struct iovec iov[2];
} Packet;

typedef struct Header {
    uint16_t checksum;
    uint16_t sequence;

} Header;


#endif //UNIXCUSTOMTRANSPORTLAYER_DUSTYNS_TRANSPORT_LAYER_H
