//
// Created by dustyn on 4/22/24.
//

#include <stdbool.h>
#include "dustyns_transport_layer.h"
#include "network_layer.h"

/*
 * Raw sockets are a powerful feature in UNIX. Raw sockets remove the kernels' implementation
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
 * Allocate a packet on the heap , being sure to allocate both io vectors inside the packet as well
 * We need to do a standard null check to ensure that allocation is not returning a null pointer
 */

Packet *allocate_packet() {
    Packet *packet = (Packet *) malloc(sizeof(Packet));

    if (packet == NULL) {
        perror("malloc");
        return NULL;
    }
    packet->ip_header = malloc(sizeof(struct iphdr));
    packet->iov[0].iov_base = malloc(HEADER_SIZE);
    packet->iov[0].iov_len = HEADER_SIZE;
    packet->iov[1].iov_base = malloc(PACKET_SIZE);
    packet->iov[1].iov_len = PACKET_SIZE;

    if (packet->iov[0].iov_base == NULL || packet->iov[1].iov_base == NULL) {
        perror("malloc");
        free(packet->iov[0].iov_base);
        free(packet->iov[1].iov_base);
        free(packet);
        return NULL;
    }

    return packet;

}

/*
 * Free packet from heap memory, check that it is not null to avoid dereferencing a null pointer, set each packet to null afterwards
 * to make sure there are no double frees.
 */
uint16_t free_packet(Packet *packet) {
    if (packet == NULL) {
        return ERROR;
    }
    if (packet->iov[0].iov_base != NULL) {
        free(packet->iov[0].iov_base);
        packet->iov[0].iov_base = NULL;
    }

    if (packet->iov[1].iov_base != NULL) {
        free(packet->iov[1].iov_base);
        packet->iov[1].iov_base = NULL;
    }

    free(packet);
    return SUCCESS;
}

/*
 * This will set the alarm for a packet timeout
 * A custom timer is allowed, however, the default will be
 * the value of the TIMEOUT macro. This will set an alarm
 * and if the system does not receive an ack in the given
 * timeframe, a sigalrm will be sent by the kernel to alert us
 * We can implement exponential back off as well.
 * Exponential backoff means each timeout we double the timeout
 * period. This can be useful to conserve resources and ensure any issues are resolved.
 * If not we will abort the sending of this packet set.
 *
 * Exponential backoff is a method to ensure we are not being too
 * aggressive and allowing time for any network issues to pass
 * This can relieve issues such as bogging the network / congestion.
 */
uint16_t set_packet_timeout(int custom_timer, int num_timeouts) {

    if (custom_timer != NULL && custom_timer > 0 && custom_timer < MAX_TIMEOUT) {
        alarm(custom_timer);
        return custom_timer;
    } else {
        if (num_timeouts == 0) {
            alarm(INITIAL_TIMEOUT);
            return INITIAL_TIMEOUT;
        } else {
            uint16_t timeout_value = (INITIAL_TIMEOUT);

            for (int i = 0; i < num_timeouts; ++i) {
                timeout_value *= 2;
            }

            if (timeout_value < MAX_TIMEOUT) {
                alarm(timeout_value);
                return timeout_value;
            } else {
                return -ERROR;
            }

        }
    }
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
        return (uint8_t) ERROR;
    } else {
        return (uint8_t) SUCCESS;
    }
}

/*
 * This will inspect an array of packets and make sure that we have all the sequencing correct.
 * Also sending out RESEND messages to the other side with the packet sequence number that will need to be sent back.
 */

uint16_t handle_ack(int socket, Packet *packets[MAX_PACKET_COLLECTION]) {
    bool sequence_received[MAX_PACKET_COLLECTION + 1] = {false}; // Initialize all to false
    int last_received = -1;
    int missing_packets = 0;
    int highest_packet_received;

    // Iterate through each packet in the collection
    for (int i = 0; i < MAX_PACKET_COLLECTION; ++i) {
        Packet *packet = packets[i];

        if (packet == NULL) break;

        Header *header = (Header *) packet->iov[0].iov_base;
        sequence_received[header->sequence] = true;
        last_received = header->sequence;
        highest_packet_received = last_received;
    }

    // Check for missing packets and send RESEND if needed
    for (int i = 0; i <= last_received; ++i) {
        if (!sequence_received[i]) {
            // Packet with sequence i is missing, send RESEND
            send_resend(socket, i);
            missing_packets += 1;
        }
    }
    if (missing_packets > 0) {

        return missing_packets;

    } else {

        if (send_ack(socket, highest_packet_received) != SUCCESS) {
            return ERROR;
        }

        return SUCCESS;

    }
}

/*
 * This function is for when a set of packets has been checked properly and an acknowledge can be sent.
 * Send the acknowledge message to the client side., return SUCCESS or ERROR depending on return value of sendmsg() call
 */
uint16_t send_ack(int socket, uint16_t max_sequence) {
    Header header = {
            ACKNOWLEDGE,
            0,
            max_sequence,
            0
    };
    struct iovec iov;
    iov.iov_base = &header;
    iov.iov_len = sizeof header;

    struct msghdr message;
    memset(&message, 0, sizeof(message));
    message.msg_iov = &iov;
    message.msg_iovlen = 1;

    ssize_t bytes_sent = sendmsg(socket, &message, 0);

    if (bytes_sent < 0) {
        return ERROR;
    } else {
        return SUCCESS;
    }

}

/*
 *  This function handles sending RESEND packets which will have no body just a header with the RESEND status, and the seq number of the missing packet
 *  Returns the seq number on success and ERROR otherwise.
 */
uint16_t send_resend(int socket, uint16_t sequence) {
    Header header = {
            RESEND,
            0,
            sequence
    };
    struct iovec iov;
    iov.iov_base = &header;
    iov.iov_len = sizeof header;

    struct msghdr message;
    memset(&message, 0, sizeof(message));
    message.msg_iov = &iov;
    message.msg_iovlen = 1;

    ssize_t bytes_sent = sendmsg(socket, &message, 0);

    if (bytes_sent < 0) {
        return ERROR;
    } else {
        return header.sequence;
    }

}

/*
 * If we notice a bad payload via XORing and comparing with the checksum, we want to fire off a packet with the status
 * CORRUPTION.
 * The client raw socket will check for and then read the sequence from that header, and if there is a CORRUPTION
 * header, then the client will read the sequence and resend that packet
 */

uint16_t handle_corruption(int socket, struct Header *head) {

    Header header = {
            CORRUPTION,
            0,
            head->sequence
    };
    struct iovec iov;
    iov.iov_base = &header;
    iov.iov_len = sizeof header;

    struct msghdr message;
    memset(&message, 0, sizeof(message));
    message.msg_iov = &iov;
    message.msg_iovlen = 1;

    ssize_t bytes_sent = sendmsg(socket, &message, 0);
    if (bytes_sent < 0) {
        return ERROR;
    } else {
        return header.sequence;
    }
}

/*
 * This function will send out of band data , which is akin to a network interrupt if you will. We will
 * allow 1 byte of OOB data to be send, could be some kind of escape or abort signal. OOB data is supposed to skip the queue
 * and come off the wire and be processed before anything else.
 */
uint16_t send_oob_data(int socket, char oob_char) {

    Header header = {
            OOB,
            0,
            0,
            OUT_OF_BAND_DATA_SIZE,
    };

    struct iovec iov;
    iov.iov_base = &header;
    iov.iov_len = sizeof header;
    iov.iov_base = &oob_char;
    iov.iov_len = OUT_OF_BAND_DATA_SIZE;

    struct msghdr message;
    memset(&message, 0, sizeof(message));
    message.msg_iov = &iov;
    message.msg_iovlen = 2;

    ssize_t bytes_sent = sendmsg(socket, &message, 0);
    if (bytes_sent < 0) {
        return ERROR;

    } else {
        return SUCCESS;
    }
}


/*
 * Function to handle sending a connection closed message to the client side of the conn
 */
uint16_t handle_close(int socket) {

    Header header = {
            CLOSE,
            0,
            0,
            0
    };

    struct iovec iov;
    iov.iov_base = &header;
    iov.iov_len = sizeof header;

    struct msghdr message;
    memset(&message, 0, sizeof(message));
    message.msg_iov = &iov;
    message.msg_iovlen = 1;

    ssize_t bytes_sent = sendmsg(socket, &message, 0);
    if (bytes_sent < 0) {
        return ERROR;
    } else {
        return SUCCESS;
    }

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

    iov[1].iov_base = (char *) &welcome_msg;
    iov[1].iov_len = PACKET_SIZE;


}