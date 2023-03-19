#ifndef _dhcp_h
#define _dhcp_h

void dhcp_handle_packet(uint8_t *data, uint32_t size);
void dhcp_discover();

#endif