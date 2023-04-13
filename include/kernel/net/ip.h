#ifndef _ip_h
#define _ip_h

#define IP_PROTOCOL_TCP 6
#define IP_PROTOCOL_UDP 17

void ip_handle_packet(uint8_t *buffer);
void ip_send_packet(uint8_t *src_ip, uint8_t *dst_ip, uint8_t protocol, uint8_t *data, uint32_t size);

#endif
