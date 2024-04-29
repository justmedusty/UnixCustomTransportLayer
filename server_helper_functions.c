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