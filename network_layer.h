//
// Created by dustyn on 4/22/24.
//
#include "dustyns_transport_layer.h"

#ifndef UNIXCUSTOMTRANSPORTLAYER_NETWORK_LAYER_H
#define UNIXCUSTOMTRANSPORTLAYER_NETWORK_LAYER_H

#define IP_HEADER_SIZE 64

uint16_t fill_ip_header(struct iphdr *ip_header, char *src_ip, char *dst_ip);

        unsigned short checksum(void *b, int len);

uint16_t get_ip_header_wire_ready(struct iphdr (*ip_header));

uint16_t get_ip_header_host_ready(struct iphdr (*ip_header));

#endif //UNIXCUSTOMTRANSPORTLAYER_NETWORK_LAYER_H
