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
#include "../include/kernel/net/net.h"
#include "../include/kernel/mem/heap.h"

#include "../include/kernel/audio/dev.h"

#define UDP_SRC_PORT_DHCP   67
#define UDP_SRC_PORT_DNS    53

udp_socket_t sockets[1024];
uint16_t socket_count = 0;

typedef struct udp_packet_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
} __attribute__((packed)) udp_packet_header;

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

    for (uint16_t i=0; i < socket_count; i++) {
        udp_socket_t *socket = &sockets[i];

        if (socket->listening && socket->port == packet->dst_port) {
            uint8_t *data = (uint8_t *) packet + sizeof(udp_packet_header);

            (*socket->listener)(data, size);
        }
    }
}

void udp_send_packet(uint8_t *dst_ip, uint16_t src_port, uint16_t dst_port, uint8_t *data, uint32_t size)
{
    dhcp_config_t *dhcp_config = net_get_dhcp_config();

    udp_packet_header *packet = (udp_packet_header *) malloc(sizeof(udp_packet_header) + size);

    packet->length = net_swap_word(sizeof(udp_packet_header) + size);
    packet->src_port = net_swap_word(src_port);
    packet->dst_port = net_swap_word(dst_port);
    packet->checksum = 0;

    memcpy((uint8_t *) packet + sizeof(udp_packet_header), data, size);

    ip_send_packet(dhcp_config->ip, dst_ip, IP_PROTOCOL_UDP, (uint8_t *) packet, sizeof(udp_packet_header) + size);

    //free(packet);
}

udp_socket_t *udp_create()
{
    udp_socket_t *socket = &sockets[socket_count];
    socket->id = socket_count;

    socket_count++;

    return socket;
}

void udp_bind(udp_socket_t *socket, uint8_t ip[4], uint16_t port)
{
    memcpy(socket->ip, ip, 4);
    socket->port = port;
}

void udp_send(udp_socket_t *socket, uint8_t *data, uint32_t size)
{
    dhcp_config_t *dhcp_config = net_get_dhcp_config();

    udp_send_packet(socket->ip, socket->id, socket->port, data, size);
}

void udp_listen(udp_socket_t *socket, void (*listener) (uint8_t *data, uint32_t size))
{
    socket->listener = listener;
    socket->listening = true;
}