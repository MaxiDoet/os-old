#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/net/dhcp.h"
#include "../include/kernel/net/dns.h"
#include "../include/kernel/mem/heap.h"

#include "../include/kernel/audio/dev.h"

#define UDP_SRC_PORT_DHCP   67
#define UDP_SRC_PORT_DNS    53

void udp_handle_packet(uint8_t *data, uint32_t size)
{
    udp_packet_header *packet = (udp_packet_header *) data;

    packet->src_port = net_swap_word(packet->src_port);
    packet->dst_port = net_swap_word(packet->dst_port);
    packet->length = net_swap_word(packet->length);

    switch(packet->src_port) {
        case UDP_SRC_PORT_DHCP:
            dhcp_handle_packet((uint8_t *) packet + sizeof(udp_packet_header), packet->length);
            break;

        case UDP_SRC_PORT_DNS:
            dns_handle_packet((uint8_t *) packet + sizeof(udp_packet_header), packet->length);
            break;
    }
}

void udp_send_packet(uint8_t *src_ip, uint8_t *dst_ip, uint16_t src_port, uint16_t dst_port, uint8_t *data, uint32_t size)
{
    udp_packet_header *packet = (udp_packet_header *) malloc(sizeof(udp_packet_header) + size);

    packet->length = net_swap_word(sizeof(udp_packet_header) + size);
    packet->src_port = net_swap_word(src_port);
    packet->dst_port = net_swap_word(dst_port);
    packet->checksum = 0;

    memcpy((uint8_t *) packet + sizeof(udp_packet_header), data, size);

    ip_send_packet(src_ip, dst_ip, (uint8_t *) packet, sizeof(udp_packet_header) + size);
}