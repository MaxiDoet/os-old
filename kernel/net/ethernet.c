#include <stdbool.h>
#include <stdint.h>

#include "../libc/include/string.h"
#include "../libc/include/mm.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/drivers/rtl8139.h"

void ethernet_send_frame(uint8_t src_mac[6], uint8_t dst_mac[6], ethertype type, uint16_t *data, uint32_t size)
{
	uint16_t *buf = (uint16_t *) malloc(mm, sizeof(etherframe_header) + size);
	memset(buf, 0x00, sizeof(etherframe_header) + size);

	etherframe_header *header = (etherframe_header *) buf;
	memcpy(header->dst_mac, dst_mac, 6);
	memcpy(header->src_mac, src_mac, 6);
	header->ether_type = type;

	memcpy(buf + sizeof(etherframe_header) - 7, data, size);

	rtl8139_send(buf, sizeof(etherframe_header) + size);
}
