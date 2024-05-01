//
// Created by dustyn on 4/22/24.
//

#include "server_helper_functions.h"
#include "dustyns_transport_layer.h"
#include "network_layer.h"

int main() {
    int sockfd;
    ssize_t recv_len;

    // Create a raw socket for custom protocol packets
    if ((sockfd = socket(AF_INET, SOCK_RAW, IP_HDRINCL)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("getting ready to listen\n");
    handle_client_connection(sockfd, inet_addr("127.0.0.1"),inet_addr("127.0.0.1"),1000);

    close(sockfd);
    return 0;
}