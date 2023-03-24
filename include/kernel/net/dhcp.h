#ifndef _dhcp_h
#define _dhcp_h

typedef struct dhcp_config_t {
    uint8_t ip[4];
    uint8_t subnet_mask[4];
    uint8_t dns[4];
    uint8_t router[4];
    uint8_t dhcp_server[4];

    bool ack;
} dhcp_config_t;

void dhcp_handle_packet(uint8_t *data, uint32_t size);
void dhcp_discover();

#endif