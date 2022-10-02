#include <stdint.h>
#include <stdbool.h>

#include "../libc/include/string.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/kernel.h"

void ip_handle_packet(uint16_t *buffer)
{
	ip_packet_header *header = (ip_packet_header *) buffer;

	//header->version = (header->version >> 4) & 0xF;
	kdebug("[ip] version: %d\r\n", header->version);
}

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

void ip_send_packet(uint8_t *dst_ip, void *data, int len) {
	void *packet = (void *) malloc(sizeof(ip_packet_header) + len);
	ip_packet_header *header = (ip_packet_header *) packet;

	header->version = IP_IPV4;
	header->ihl = 5;
	header->tos = 0;
	header->length = sizeof(ip_packet_header) + len;
	header->id = 0;
	header->flags = 0;
	header->ttl = 64;
	header->protocol = 17;

	memcpy(header->src_ip, arp_get_ip(), 4);
	memcpy(header->dst_ip, dst_ip, 4);

	// Copy data
	void *packet_data = (void *) packet + header->ihl * 4;
	memcpy(packet_data, data, len);

	// Calculate checksum
	header->checksum = 0;
	header->checksum = ip_calculate_checksum(header);

	// Resolve ip
	uint8_t dst_mac[6];
	memcpy(dst_mac, arp_resolve_mac(dst_ip), 6);

	// Send packet
	ethernet_send_frame(arp_get_mac(), dst_mac, ETHERTYPE_IPV4, (uint16_t *) packet, sizeof(ip_packet_header) + len);
}
