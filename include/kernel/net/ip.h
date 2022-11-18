#ifndef _ip_h
#define _ip_h

void ip_handle_packet(uint8_t *buffer);
void ip_send_packet(uint8_t *dst_ip, uint8_t *data, uint32_t size);

#endif
