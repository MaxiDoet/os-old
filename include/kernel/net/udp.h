#ifndef _udp_h
#define _udp_h

typedef struct udp_socket_t {
	uint16_t id;
	uint16_t port;
	uint8_t ip[4];

	bool listening;
	void (*listener) (uint8_t *data, uint32_t size);
} udp_socket_t;

void udp_handle_packet(uint8_t *data, uint32_t size);
void udp_send_packet(uint8_t *src_ip, uint8_t *dst_ip, uint16_t src_port, uint16_t dst_port, uint8_t *data, uint32_t size);

/* Socket */
udp_socket_t *udp_create();
void udp_bind(udp_socket_t *socket, uint8_t ip[4], uint16_t port);
void udp_send(udp_socket_t *socket, uint8_t *data, uint32_t size);
void udp_listen(udp_socket_t *socket, void (*listener) (uint8_t *data, uint32_t size));

#endif
