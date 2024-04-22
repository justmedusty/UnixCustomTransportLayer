//
// Created by dustyn on 4/22/24.
//

#include "network_layer.h"



/*
 * This is a standard algorithm for IP checksums.
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