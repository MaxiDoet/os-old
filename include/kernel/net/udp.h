#ifndef _udp_h
#define _udp_h

typedef struct udp_packet_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
} __attribute__((packed)) udp_packet_header;

void udp_handle_packet(uint8_t *data, uint32_t size);
void udp_send_packet(uint8_t *src_ip, uint8_t *dst_ip, uint16_t src_port, uint16_t dst_port, uint8_t *data, uint32_t size);

#endif
