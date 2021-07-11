#include <stdint.h>

#include "../libc/include/string.h"
#include "../libc/include/mm.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/net/arp.h"
#include "../config.h"

static uint8_t ip[4];
static uint8_t mac[6];

static uint8_t mac_cache[100][6];
static uint8_t ip_cache[100][4];
static int cache_index;

void arp_set_mac(uint8_t addr[6])
{
	memcpy(&mac, &addr, 6);
}

void arp_set_ip(uint8_t addr[4])
{
	memcpy(&ip, &addr, 4);
}

void arp_handle_packet(arp_packet *packet)
{
	switch(packet->operation_type) {
		case 0x0100:
			// Request
			break;
		case 0x0200:
			// Reply
			memcpy(&mac_cache[cache_index], packet->src_mac, 6);
			memcpy(&ip_cache[cache_index], packet->src_ip, 4);
			break;
	}
}

void arp_request_mac(uint8_t ip[4])
{
	arp_packet *packet = (arp_packet *) malloc(mm, sizeof(arp_packet));
	memset((uint16_t *) packet, 0x00, sizeof(arp_packet));

	packet->hardwareaddress_type = 0x0100;
	packet->protocol_type = 0x0008;
	packet->hardwareaddress_length = 6;
	packet->networkaddress_length = 4;
	packet->operation_type = 0x0100;
	
	memcpy(packet->src_mac, &mac, 6);
	memcpy(packet->src_ip, &ip, 4);
	memset(packet->dst_mac, 0xFF, 6);
	memcpy(packet->dst_ip, &ip, 4);

	uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	ethernet_send_frame(mac, broadcast_mac, ETHERTYPE_ARP, (uint16_t *) packet, sizeof(arp_packet));
}
