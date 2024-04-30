//
// Created by dustyn on 4/22/24.
//

#include "server_helper_functions.h"
#include "dustyns_transport_layer.h"
#include "network_layer.h"

int main() {
    int sockfd;
    ssize_t recv_len;

    // Create a raw socket for ICMP packets
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("getting ready to listen\n");
    handle_client_connection(sockfd, inet_addr("192.168.68.59"),inet_addr("192.168.68.59"),1000);

    close(sockfd);
    return 0;
}