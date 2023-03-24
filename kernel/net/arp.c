#include <stdbool.h>
#include <stdint.h>

#include "../libc/include/string.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/kernel/net/arp.h"
#include "../config.h"

static uint8_t ip[4];
static uint8_t mac[6];

static uint8_t mac_cache[100][6];
static uint8_t ip_cache[100][4];
static int cache_index;

uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void arp_reply(arp_packet *packet);

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
	#ifdef NET_DEBUG_ARP
	kdebug("[net] ARP | ");
	#endif

	switch(packet->operation_type) {
		case 0x0100:
			// Request
			#ifdef NET_DEBUG_ARP
			kdebug("Request from ");
			print_mac(packet->src_mac);
			kdebug(" to: ");
			print_ip(packet->dst_ip);
			kdebug("\r\n");
			#endif

			arp_reply(packet);
			break;
		case 0x0200:
			// Reply
			#ifdef NET_DEBUG_ARP
			kdebug("Reply from: ");
			print_ip(packet->src_ip);
			kdebug(" (");
			print_mac(packet->src_mac);
			kdebug(") to: ");
			print_ip(ip);
			kdebug("\r\n");
			#endif

			memcpy(&mac_cache[cache_index], packet->src_mac, 6);
			memcpy(&ip_cache[cache_index], packet->src_ip, 4);
			cache_index++;
			break;
	}
}

void arp_request_mac(uint8_t addr[4])
{
	#ifdef NET_DEBUG_ARP
	kdebug("[net] ARP | Requesting ");
	print_ip(addr);
	kdebug("\r\n");
	#endif

	arp_packet *packet = (arp_packet *) malloc(sizeof(arp_packet));

	packet->hardwareaddress_type = 0x0100;
	packet->protocol_type = 0x0008;
	packet->hardwareaddress_length = 6;
	packet->networkaddress_length = 4;
	packet->operation_type = 0x0100;

	memset(packet->dst_mac, 0x00, 6);
	memcpy(packet->dst_ip, addr, 4);

	ethernet_send_frame(mac, broadcast_mac, ETHERTYPE_ARP, (uint8_t *) packet, sizeof(arp_packet));
}

void arp_reply(arp_packet *packet)
{
	arp_packet *reply_packet = (arp_packet *) malloc(sizeof(arp_packet));

	reply_packet->hardwareaddress_type = 0x0100;
	reply_packet->protocol_type = 0x0008;
	reply_packet->hardwareaddress_length = 6;
	reply_packet->networkaddress_length = 4;
	reply_packet->operation_type = 0x0200;

	memcpy(reply_packet->src_mac, &mac, 6);
	memcpy(reply_packet->src_ip, &ip, 4);
	memcpy(reply_packet->dst_mac, packet->src_mac, 6);
	memcpy(reply_packet->dst_ip, packet->src_ip, 4);

	ethernet_send_frame(mac, broadcast_mac, ETHERTYPE_ARP, (uint8_t *) reply_packet, sizeof(arp_packet));
}

void arp_broadcast_mac(uint8_t addr[4])
{
	arp_packet *packet = (arp_packet *) malloc(sizeof(arp_packet));

	packet->hardwareaddress_type = 0x0100;
	packet->protocol_type = 0x0008;
	packet->hardwareaddress_length = 6;
	packet->networkaddress_length = 4;
	packet->operation_type = 0x0200;

	memcpy(packet->src_mac, &mac, 6);
	memcpy(packet->src_ip, &ip, 4);
	memcpy(packet->dst_mac, arp_get_mac(addr), 6);
	memcpy(packet->dst_ip, addr, 4);

	ethernet_send_frame(mac, arp_resolve_mac(addr), ETHERTYPE_ARP, (uint8_t *) packet, sizeof(arp_packet));
}

uint8_t *arp_resolve_mac(uint8_t *ip)
{
	for (int i=0; i < cache_index; i++) {
		for (int ip_index=0; ip_index < 4; ip_index++) {
			if (ip_cache[cache_index][ip_index] != ip[ip_index]) {
				break;
			}

			if (ip_index == 3) {
				#ifdef NET_DEBUG_ARP
				kdebug("[net] ARP | Resolved ");
				print_mac(mac_cache[i]);
				kdebug("from cache\r\n");
				#endif

				return mac_cache[cache_index];
			}
		}
	}

	arp_request_mac(ip);

	uint8_t result[6];
	while (result == 0) {
		for (int i=0; i < cache_index; i++) {
			for (int ip_index=0; ip_index < 4; ip_index++) {
				if (ip_cache[cache_index][ip_index] != ip[ip_index]) {
					break;
				}

				if (ip_index == 3) {
					return mac_cache[cache_index];
				}
			}
		}
	}
}

uint8_t *arp_get_ip() {
	return ip;
}

uint8_t *arp_get_mac() {
	return mac;
}
