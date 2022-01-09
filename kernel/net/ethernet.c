#include <stdbool.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../libc/include/mm.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/drivers/rtl8139.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/swap.h"

void ethernet_send_frame(uint8_t src_mac[6], uint8_t dst_mac[6], ethertype type, uint16_t *data, uint32_t size)
{
	uint16_t *buf = (uint16_t *) malloc(sizeof(etherframe_header) + size);
	memset(buf, 0x00, sizeof(etherframe_header) + size);

	etherframe_header *header = (etherframe_header *) buf;
	memcpy(header->dst_mac, dst_mac, 6);
	memcpy(header->src_mac, src_mac, 6);
	header->ether_type = net_swap_word(type);

	memcpy(buf + sizeof(etherframe_header) - 7, data, size);

	rtl8139_send(buf, sizeof(etherframe_header) + size);
}

void ethernet_handle_frame(uint16_t *buffer, uint16_t length)
{
	etherframe_header *frame_header = (etherframe_header *) (buffer + 2);
	frame_header->ether_type = net_swap_word(frame_header->ether_type);

	kdebug("[net] ether_type: %x dst_mac: ", frame_header->ether_type);

	for (int i=0; i < 6; i++) {
		kdebug("%x%s", frame_header->dst_mac[i], ((i < 5) ? ":" : " src_mac: "));
	}

	for (int i=0; i < 6; i++) {
		kdebug("%x%s", frame_header->src_mac[i], ((i < 5) ? ":" : "\r\n"));
	}

	uint32_t *packet = (uint32_t *) ((uint32_t) frame_header + sizeof(etherframe_header));

	switch(frame_header->ether_type) {
		case ETHERTYPE_ARP: ;
			arp_handle_packet((arp_packet *) packet);
			break;
		case ETHERTYPE_IPV4: ;
			ip_handle_packet((void *) packet);
			break;
	}
}

