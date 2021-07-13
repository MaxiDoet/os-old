#ifndef _arp_h
#define _arp_h

typedef struct arp_packet {
	uint16_t hardwareaddress_type;
	uint16_t protocol_type;
	uint8_t hardwareaddress_length;
	uint8_t networkaddress_length;
	uint16_t operation_type;
	uint8_t src_mac[6];
	uint8_t src_ip[4];
	uint8_t dst_mac[6];
	uint8_t dst_ip[4];
} __attribute__((packed)) arp_packet;

void arp_set_mac(uint8_t addr[6]);
void arp_set_ip(uint8_t addr[4]);
void arp_handle_packet(arp_packet *packet);
void arp_request_mac(uint8_t addr[4]);

#endif
