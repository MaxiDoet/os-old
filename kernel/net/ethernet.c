#include <stdbool.h>
#include <stdint.h>

#include "../config.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/drivers/rtl8139.h"
#include "../include/kernel/net/net.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/utils.h"

void ethernet_send_frame(uint8_t src_mac[6], uint8_t dst_mac[6], ethertype type, uint8_t *data, uint32_t size)
{
	uint8_t *buf = (uint8_t *) malloc(sizeof(etherframe_header) + size);
	memset(buf, 0x00, sizeof(etherframe_header) + size);

	etherframe_header *header = (etherframe_header *) buf;
	memcpy(header->dst_mac, dst_mac, 6);
	memcpy(header->src_mac, src_mac, 6);

	header->ether_type = net_swap_word(type);

	memcpy(buf + sizeof(etherframe_header), data, size);

	/*
	kdebug("tx: ");
	for (uint32_t i=0; i < size; i++) {
		kdebug("%x ", data[i]);
	}
	kdebug("\r\n");
	*/

	rtl8139_send(buf, sizeof(etherframe_header) + size);

	//free(buf);
}

void ethernet_handle_frame(uint8_t *buffer, uint32_t size)
{
	etherframe_header *frame_header = (etherframe_header *) buffer;
	frame_header->ether_type = net_swap_word(frame_header->ether_type);
	uint8_t *packet = (uint8_t *) ((uint32_t) frame_header + sizeof(etherframe_header));

	switch(frame_header->ether_type) {
		case ETHERTYPE_ARP: ;
			arp_handle_packet((arp_packet *) packet);
			break;

		case ETHERTYPE_IPV4: ;
			ip_handle_packet(packet);
			break;
	}
}

