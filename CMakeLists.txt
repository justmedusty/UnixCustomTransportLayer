cmake_minimum_required(VERSION 3.28)
project(UnixCustomTransportLayer C)

set(CMAKE_C_STANDARD 11)

add_executable(UnixCustomTransportLayer server_main.c
        network_layer.c
        network_layer.h
        dustyns_transport_layer.c
        dustyns_transport_layer.h
        server_helper_functions.c
        server_helper_functions.h)
