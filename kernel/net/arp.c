#include <stdbool.h>
#include <stdint.h>

#include "../libc/include/string.h"
#include "../libc/include/mm.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/kernel/net/arp.h"
#include "../config.h"

static uint8_t ip[4];
static uint8_t mac[6];

static uint8_t mac_cache[100][6];
static uint8_t ip_cache[100][4];
static int cache_index;

void print_mac(uint8_t addr[6])
{
	for (int i=0; i < 6; i++) {
		kdebug("%x%s", addr[i], ((i < 5) ? ":" : ""));
	}
}

void print_ip(uint8_t addr[4])
{
	for (int i=0; i < 4; i++) {
		kdebug("%d%s", addr[i], ((i < 3) ? "." : ""));
	}
}

void arp_set_mac(uint8_t addr[6])
{
	memcpy(&mac, addr, 6);
}

void arp_set_ip(uint8_t addr[4])
{
	memcpy(&ip, addr, 4);
}

void arp_handle_packet(arp_packet *packet)
{
	switch(packet->operation_type) {
		case 0x0100:
			// Request
			kdebug("arp request from: ");
			print_mac(packet->src_mac);
			kdebug("\r\n");
			break;
		case 0x0200:
			// Reply
			kdebug("arp reply from: ");
			print_ip(packet->src_ip);
			kdebug("\r\n");
			memcpy(&mac_cache[cache_index], packet->src_mac, 6);
			memcpy(&ip_cache[cache_index], packet->src_ip, 4);
			break;
	}
}

void arp_request_mac(uint8_t addr[4])
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
	memcpy(packet->dst_ip, addr, 4);

	uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	kdebug("arp requesting mac: ");
	print_ip(addr);
	kdebug("\r\n");
	ethernet_send_frame(mac, broadcast_mac, ETHERTYPE_ARP, (uint16_t *) packet, sizeof(arp_packet));
}
