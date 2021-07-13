#ifndef _ip_h
#define _ip_h

typedef struct ip_packet {
	uint8_t length : 4;
	uint8_t version : 4;
	uint8_t priority;
	uint16_t total_length;
	uint16_t id;
	uint16_t flags;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint32_t src_ip;
	uint32_t dst_ip;
} __attribute__((packed)) ip_packet;

void ip_handle_packet(ip_packet *packet);

#endif
