//
// Created by dustyn on 4/22/24.
//

#include "network_layer.h"



/*
 * This is a standard algorithm for IP checksums.
 * We will use to verify layer 3 headers to ensure
 * that the IP header has not been corrupted during
 * transport.If it is no good, the packet will be thrown out.
 */


unsigned short checksum(void *b, int len) {
    // Cast the input pointer to an unsigned short pointer
    unsigned short *buf = b;

    // Initialize the sum variable
    unsigned int sum = 0;

    // Declare a variable to hold the checksum result
    unsigned short result;

    // Iterate over the data buffer, summing up 16-bit words
    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++; // Add the value pointed to by buf to sum, then move buf to the next 16-bit word
    }

    // If the length is odd, add the last byte separately
    if (len == 1) {
        sum += *(unsigned char*)buf; // Add the value of the last byte to sum
    }

    // Add any carry bits to the lower 16 bits of sum
    sum = (sum >> 16) + (sum & 0xFFFF);

    // Add carry from the addition above to the lower 16 bits of sum
    sum += (sum >> 16);

    // Take the one's complement of sum to obtain the checksum result
    result = ~sum;

    // Return the checksum result
    return result;
}

/*
 * Since we are using IPPROTO_RAW, we will need to fill out IP headers ourselves here.
 * We will need to do our own checksums as well. We need to specify the source ip, the dest ip,
 * the ttl, the version, the packet size, everything!
 *
 * In the end, we will fill in our ip_header checksum for verification of the IP header at layer 3.
 */

void fill_ip_header(struct iphdr *ip_header, char *src_ip, char *dst_ip) {
    ip_header->ihl = 5; // Header length (in 32-bit words)
    ip_header->version = 4; // IPv4
    ip_header->tos = 0; // Type of service
    ip_header->tot_len = htons(PACKET_SIZE); // Total length of the packet
    ip_header->id = htons(12345); // Identification
    ip_header->frag_off = 0; // Fragmentation offset
    ip_header->ttl = 64; // Time to live
    ip_header->protocol = IPPROTO_RAW; // Protocol
    ip_header->check = 0; // Checksum (0 for now, will be calculated later)
    ip_header->saddr = inet_addr(src_ip); // Source IP address
    ip_header->daddr = inet_addr(dst_ip); // Destination IP address

    ip_header->check = checksum(ip_header, sizeof(struct iphdr));


}