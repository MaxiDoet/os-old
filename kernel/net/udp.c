#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/mem/heap.h"

void udp_handle_packet(uint8_t *packet, uint32_t size)
{
    kdebug("udp handle packet\r\n");
}

void udp_send_packet(uint8_t *dst_ip, uint16_t src_port, uint16_t dst_port, uint8_t *data, uint32_t size)
{
    udp_packet_header *packet = (udp_packet_header *) malloc(sizeof(udp_packet_header)) + size;

    packet->length = sizeof(udp_packet_header) + size;
    packet->src_port = src_port;
    packet->dst_port = dst_port;
    packet->checksum = 0;

    memcpy(packet + sizeof(packet), data, size);

    ip_send_packet(dst_ip, (uint8_t *) packet, sizeof(udp_packet_header) + size);
}