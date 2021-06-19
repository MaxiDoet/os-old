#ifndef _arp_h
#define _arp_h

typedef struct arp_packet {
	uint16_t hardwareaddress_type;
	uint16_t networkaddress_type;
	uint8_t hardwareaddress_length;
	uint8_t networkaddress_length;
	uint16_t operation_type;
	uint64_t src_mac : 48;
	uint32_t src_ip;
	uint64_t dst_mac : 48;
	uint32_t dst_ip;
} __attribute__((packed)) arp_packet;

#endif
