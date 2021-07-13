#ifndef _ethernet_h
#define _ethernet_h

typedef enum {
	ETHERTYPE_IPV4 = 0x800,
	ETHERTYPE_ARP = 0x608
} ethertype;

typedef struct etherframe_header {
        uint8_t dst_mac[6];
        uint8_t src_mac[6];
        uint16_t ether_type;
} __attribute__((packed)) etherframe_header;

void ethernet_send_frame(uint8_t src_mac[6], uint8_t dst_mac[6], ethertype type, uint16_t *data, uint32_t size);

#endif
