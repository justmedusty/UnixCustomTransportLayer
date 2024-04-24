//
// Created by dustyn on 4/22/24.
//
#include "dustyns_transport_layer.h"
#ifndef UNIXCUSTOMTRANSPORTLAYER_NETWORK_LAYER_H
#define UNIXCUSTOMTRANSPORTLAYER_NETWORK_LAYER_H
void fill_ip_header(struct iphdr *ip_header, char *src_ip, char *dst_ip);
unsigned short checksum(void *b, int len);
#endif //UNIXCUSTOMTRANSPORTLAYER_NETWORK_LAYER_H
