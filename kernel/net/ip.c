#include <stdint.h>
#include <stdbool.h>

#include "../libc/include/string.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/kernel.h"

typedef struct ip_packet_header {
	uint8_t version : 4;
	uint8_t ihl : 4;
	uint8_t tos;
	uint16_t length;
	uint16_t id;
	uint16_t flags;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint8_t src_ip[4];
	uint8_t dst_ip[4];
} __attribute__((packed)) ip_packet_header;

#define IP_PACKET_HEADER_VERSION_IPV4 4

#define IP_PACKET_HEADER_PROTOCOL_TCP 6 
#define IP_PACKET_HEADER_PROTOCOL_UDP 17

uint16_t ip_calculate_checksum(ip_packet_header *header) {
	int size = sizeof(ip_packet_header) / 2;
	uint32_t carry;
	uint32_t sum;

	for (int i=0; i < size; i++) {
		sum += ((uint16_t *) header)[i];
	}

	carry = sum >> 16;
	sum &= 0x0000FFFF;
	sum += carry;

	return ~sum;
}

void ip_handle_packet(uint8_t *buffer)
{
	ip_packet_header *header = (ip_packet_header *) buffer;

	/* Fix byte order */
	uint8_t version_ihl = net_swap_byte((header->version << 4) | header->ihl);
	uint16_t length = net_swap_word(header->length);
	
	header->version = ((version_ihl & 0xF0) >> 4);
	header->ihl = (version_ihl & 0x0F);

	kdebug("[ip] handle packet | version: %d\r\n", header->version);

	if (header->version != IP_PACKET_HEADER_VERSION_IPV4) {
		return;
	}

	uint16_t packet_size = length - sizeof(ip_packet_header);

	switch(header->protocol) {
		case IP_PACKET_HEADER_PROTOCOL_TCP:
			break;

		case IP_PACKET_HEADER_PROTOCOL_UDP:
			udp_handle_packet(buffer + (header->ihl * 4), packet_size);
			break;
	}
}

void ip_send_packet(uint8_t *dst_ip, uint8_t *data, uint32_t size) {
	uint8_t *packet = (uint8_t *) malloc(sizeof(ip_packet_header) + size);
	ip_packet_header *header = (ip_packet_header *) packet;

	header->version = IP_PACKET_HEADER_VERSION_IPV4;
	header->ihl = 5;
	header->tos = 0;
	header->length = sizeof(ip_packet_header) + size;
	header->id = 0;
	header->flags = 0;
	header->ttl = 64;
	header->protocol = 17;

	memcpy(header->src_ip, arp_get_ip(), 4);
	memcpy(header->dst_ip, dst_ip, 4);

	// Copy data
	uint8_t *packet_data = (uint8_t *) packet + header->ihl * 4;
	memcpy(packet_data, data, size);

	// Calculate checksum
	header->checksum = 0;
	header->checksum = ip_calculate_checksum(header);

	// Resolve ip
	uint8_t dst_mac[6];
	memcpy(dst_mac, arp_resolve_mac(dst_ip), 6);

	// Send packet
	ethernet_send_frame(arp_get_mac(), dst_mac, ETHERTYPE_IPV4, packet, sizeof(ip_packet_header) + size);
}
