#ifndef _udp_h
#define _udp_h

typedef struct udp_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
} __attribute__((packed)) udp_header;

udp_handle_packet(uint16_t *packet);

#endif
