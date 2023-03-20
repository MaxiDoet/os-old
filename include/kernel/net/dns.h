#ifndef _dns_h
#define _dns_h

void dns_handle_packet(uint8_t *data, uint32_t size);
void dns_request(uint8_t *dns_server_ip, char *cname);

#endif