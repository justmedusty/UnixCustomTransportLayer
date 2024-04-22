//
// Created by dustyn on 4/22/24.
//

#include "dustyns_transport_layer.h"

/*
 * Raw sockets are a powerful feature in UNIX. Raw sockets remove the kernels implementation
 * of the network layer protocols from the equation, and what you end up with is your own transport
 * level playground. Because we will be handling the entire transport layer, we will use sendmsg and recvmsg instead of our usual send/recv.
 * sendmsg and recvmsg alongside a msgheader structure. Because we will be working with the network layer directly here,
 * we will need to pass message metadata in headers such as its size and maybe some kind of packet identifier for acknowledgement
 * from the receiver. We will go through this together.
 *
 * The network layers purpose is getting a packet from point A to point B, ensuring there wasn't any data corruption or
 * the like will be up to us. We can implement some basic checks and balances.
*/


/*
 * This is a standard algorithm for IP checksums.
 */


Packet* allocatePacket(){
    Packet *packet = (Packet *) malloc(sizeof (Packet));

    if(packet == NULL){
        perror("malloc");
        return NULL;
    }
    packet->iov[0].iov_base = malloc(HEADER_SIZE);
    packet->iov[0].iov_len = HEADER_SIZE;
    packet->iov[1].iov_base = malloc(PACKET_SIZE);
    packet->iov[1].iov_len = PACKET_SIZE;

    if( packet->iov[0].iov_base == NULL ||  packet->iov[1].iov_base == NULL){
        perror("malloc");
        free(packet->iov[0].iov_base);
        free(packet->iov[1].iov_base);
        free(packet);
        return NULL;
    }

    return packet;

}


/*
 * This will be our checksum function, it is going to iterate through bytes in the data and bitwise XOR them.
 * This means that if the bit matches, 0 is returned.
 * A non-zero value on verification indicates that bits have changed.
 * This is a simple way to implement a checksum in our home-grown transport layer.
 *
 * Visual Example:
 *
 * XORd checksum = 1001 0101
 * Received Data = 1011 0101
 * Result would  = 0010 0000
 *
 * The result is not 0
 * The data has changed
 *
 * Nice and simple, we like that around here
 */

uint16_t calculate_checksum(char *data[], size_t length) {

    uint16_t checksum = 0;

    for (size_t i = 0; i < length; i++) {
        char *byte = data[i];
        checksum ^= *byte;
    }
    return checksum;
}

/*
 * Here will be a function for verifying the checksum when we receive one in a client header message
 * It will return either 0 or -1, checksum good, or checksum not good.
 */

uint8_t compare_checksum(char data[], size_t length, uint16_t received_checksum) {

    uint16_t new_checksum = calculate_checksum(&data, length);
    if ((new_checksum ^ received_checksum) != 0) {
        return (uint8_t) CHECKSUM_NOT_GOOD;
    } else {
        return (uint8_t) CHECKSUM_GOOD;
    }
}


// Function to handle ACK event
void handle_ack(int socket) {
    // Implement ACK handling logic
}


void handle_corruption(int socket, char *data, size_t length, uint16_t received_checksum) {
    uint16_t checksum = calculate_checksum(&data,length);
    uint16_t header = htons(checksum);
    struct iovec iov[2];
    iov[0].iov_base = &header;
    iov[0].iov_len =


}

// Function to handle close event
void handle_close(int socket) {

}
/*
 * This is our conn handler function; since we are using raw sockets, there is no transport layer. WE are the transport layer. We will do
 * some basic headers to get some metadata about the incoming messages. There will be no retransmission automatically this is all done by the
 * transport layer. We can implement our own logic to resend messages after a specific timeout time, however for this we will keep it simple.
 * We will just tack some basic metadata like message size / message length. We could even do some kind of checksum. We can implement a basic
 * checksum with a simple bitwise XOR operation.
 *
 * If you do not remember, the bitwise XOR will compare 2 bits, let's say 0 and 1, and will return 1 if they were different, and 0 if they are the same.
 * This will handle basic data corruption. It, of course, is not foolproof, but it is very simple and very lightweight.
 *
 *
 *
 */

void handle_client_connection(int socket) {
    char msg_buffer[PACKET_SIZE];
    char hdr_buffer[HEADER_SIZE];
    struct msghdr;
    struct iovec iov[2];
    uint16_t checksum;
    const char welcome_msg[] = "Welcome to the raw socket server, we are building our own transport layer on top of the IP/network layer of the OSI !";
    checksum = (&welcome_msg, strlen(welcome_msg));
    uint16_t header = htons(checksum);
    iov[0].iov_base = &header;
    iov[0].iov_len = HEADER_SIZE;

    iov[1].iov_base = (char*) &welcome_msg;
    iov[1].iov_len = PACKET_SIZE;


}