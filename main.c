//
// Created by dustyn on 4/22/24.
//

#include "server_helper_functions.h"
#include "dustyns_transport_layer.h"
#include "network_layer.h"



int main() {

    int listener;
    char buffer[PACKET_SIZE];
    int new_fd;
    struct sockaddr_storage client_address;
    socklen_t addr_len;
    struct pollfd poll_listen;
    struct sigaction sa;
    char client_ip[INET6_ADDRSTRLEN];

    listener = get_listener();

    if (listener <= 0) {
        exit(EXIT_FAILURE);
    }

    poll_listen.fd = listener;
    poll_listen.events = POLLIN;

    if (poll(&poll_listen, 1, -1) < 0) {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = signal_child_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    for (;;) {

        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
            fprintf(stderr, "Error on signal action");
            exit(EXIT_FAILURE);
        }

        if (poll_listen.revents == POLLIN) {

            new_fd = accept(listener, (struct sockaddr *) &client_address, &addr_len);
            if (new_fd < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            inet_ntop(client_address.ss_family, get_internet_addresses((struct sockaddr *) &client_address), client_ip,
                      INET6_ADDRSTRLEN);
            printf("New connection on socket %d from address %s\n", new_fd, client_ip);

            pid_t child = fork();

            if (child < 0) {
                perror("fork");
            }
            if (child == 0) {
                handle_client_connection(new_fd);
                exit(EXIT_SUCCESS);
            } else {
                close(new_fd);
                continue;
            }


        }
    }


}