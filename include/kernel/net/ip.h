#ifndef _ip_h
#define _ip_h

#define IP_IPV4 4

typedef struct ip_packet_header {
	uint8_t version : 4;
	uint8_t ihl : 4;
	uint8_t tos;
	uint16_t length;
	uint16_t id;
	uint16_t flags;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint8_t src_ip[4];
	uint8_t dst_ip[4];
} __attribute__((packed)) ip_packet_header;

void ip_handle_packet(ip_packet_header *header);
void ip_send_packet(uint8_t *dst_ip, void *data, int len);

#endif
