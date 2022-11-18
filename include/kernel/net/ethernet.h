#ifndef _ethernet_h
#define _ethernet_h

typedef enum {
	ETHERTYPE_IPV4 = 0x800,
	ETHERTYPE_IPV6 = 0x86DD,
	ETHERTYPE_ARP = 0x806
} ethertype;

typedef struct etherframe_header {
        uint8_t dst_mac[6];
        uint8_t src_mac[6];
        uint16_t ether_type;
} __attribute__((packed)) etherframe_header;

void ethernet_send_frame(uint8_t src_mac[6], uint8_t dst_mac[6], ethertype type, uint8_t *data, uint32_t size);
void ethernet_handle_frame(uint8_t *buffer, uint32_t length);

#endif
