#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/net/arp.h"
#include "../libc/include/string.h"
#include "../config.h"

static uint8_t mac_cache[100][6];
static uint8_t ip_cache[100][4];
static int cache_index;

void arp_handle_packet(arp_packet *packet)
{
	switch(packet->operation_type) {
		case 0x0100:
			// Request
			break;
		case 0x0200:
			// Reply
			memcpy(&mac_cache, packet->src_mac, 6);
			memcpy(&ip_cache, packet->src_ip, 4);
			break;
	}
}
