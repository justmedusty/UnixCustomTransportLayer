
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
#define PAYLOAD_SIZE 512
#define HEADER_SIZE 64
#define PACKET_SIZE ((sizeof (struct iphdr) + HEADER_SIZE + PAYLOAD_SIZE))
#define MAX_PACKET_COLLECTION 1000
#define OUT_OF_BAND_DATA_SIZE 1
#define ACKNOWLEDGE "ACK"
#define CORRUPTION "BAD_DATA"
#define RESEND "RESEND"
#define CLOSE "DISCONNECT"
#define OOB "OUT_OF_BAND"
#define CONNECTION_CLOSED 2
#define INITIAL_TIMEOUT 10
#define MAX_TIMEOUT 120

#define SUCCESS 0
#define ERROR 1


typedef struct Packet {
    struct iphdr ip_header;
    struct iovec iov[2];
} Packet;


typedef struct Header {
    char status[20];
    uint16_t checksum;
    uint16_t sequence;
    uint16_t msg_size;

} Header;

uint16_t handle_ack(int socket, Packet *packets[MAX_PACKET_COLLECTION]);
Packet *allocate_packet();
uint16_t free_packet(Packet *packet);
uint8_t compare_checksum(char data[], size_t length, uint16_t received_checksum);
uint16_t calculate_checksum(char *data[], size_t length);
void handle_client_connection(int socket);
uint16_t send_resend(int socket, uint16_t sequence);
uint16_t send_ack(int socket, uint16_t sequence);
uint16_t handle_close(int socket);
uint16_t handle_corruption(int socket, struct Header *head);





#endif //UNIXCUSTOMTRANSPORTLAYER_DUSTYNS_TRANSPORT_LAYER_H
