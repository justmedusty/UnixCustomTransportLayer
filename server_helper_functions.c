//
// Created by dustyn on 4/22/24.
//

#include "server_helper_functions.h"

void *get_internet_addresses(struct sockaddr *sock_address) {

    if (sock_address->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sock_address)->sin_addr);
    } else {
        return &(((struct sockaddr_in6 *) sock_address)->sin6_addr);
    }
}

void signal_child_handler(int socket) {

    int saved_error = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_error;

}

int get_listener() {

    int listener;
    struct addrinfo hints, *server, *pointer;
    int yes = 1;

    hints.ai_socktype = SOCK_RAW;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_RAW;
    int return_value;

    if ((return_value = getaddrinfo(NULL, NULL, &hints, &server)) < 0) {
        gai_strerror(return_value);
        exit(EXIT_FAILURE);
    }

    for (pointer = server; pointer != NULL; pointer = pointer->ai_next) {

        if ((listener = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol) < 0)) {
            perror("socket");
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, pointer->ai_addr, pointer->ai_addrlen) < 0) {
            perror("bind");
            continue;
        }

        break;

    }

    if (pointer == NULL) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server);

    if (listen(listener, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return listener;

}