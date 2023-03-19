#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/net/dhcp.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../libc/include/string.h"

#define DHCP_PACKET_OP_REQUEST  1
#define DHCP_PACKET_OP_REPLY    2

#define DHCP_PACKET_HTYPE_ETHERNET 1

#define DHCP_PACKET_FLAG_BROADCAST (1 << 15)

#define DHCP_OPTIONS_CODE_SUBNET_MASK   1
#define DHCP_OPTIONS_CODE_ROUTER        3
#define DHCP_OPTIONS_CODE_DNS           6
#define DHCP_OPTIONS_CODE_MESSAGE_TYPE  53
#define DHCP_OPTIONS_CODE_END           255

#define DHCP_OPTIONS_MESSAGE_TYPE_DISCOVER  1
#define DHCP_OPTIONS_MESSAGE_TYPE_OFFER     2
#define DHCP_OPTIONS_MESSAGE_TYPE_REQUEST   3
#define DHCP_OPTIONS_MESSAGE_TYPE_DECLINE   4
#define DHCP_OPTIONS_MESSAGE_TYPE_ACK       5

typedef struct dhcp_packet {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint8_t ciaddr[4];
    uint8_t yiaddr[4];
    uint8_t siaddr[4];
    uint8_t giaddr[4];
    uint8_t chaddr[16];
    char sname[64];
    char file[128];
    uint8_t magic_cookie[4];
    uint8_t options[312];
} __attribute__((packed)) dhcp_packet;

typedef struct dhcp_option_t {
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) dhcp_option_t;

typedef struct dhcp_config_t {
    uint8_t ip[4];
    uint8_t subnet_mask[4];
    uint8_t dns[4];
    uint8_t router[4];
} dhcp_config_t;

dhcp_config_t dhcp_config;

uint8_t *dhcp_read_option(dhcp_option_t *options, uint8_t type)
{
    dhcp_option_t *option = options;

    while(options->type != 0) {
        if (option->type == type) {
            return ((uint8_t *) option + sizeof(dhcp_option_t));
        }

        option = (dhcp_option_t *) ((uint8_t *) option + sizeof(dhcp_option_t) + option->length);
    }
    
    return 0;
}

void dhcp_handle_packet(uint8_t *data, uint32_t size)
{
    dhcp_packet *packet = (dhcp_packet *) data;

    if (packet->op == DHCP_PACKET_OP_REQUEST) {
    } else if (packet->op == DHCP_PACKET_OP_REPLY) {
        uint8_t message_type = 0;
        if (message_type = *dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_MESSAGE_TYPE)) {
            switch(message_type) {
                case DHCP_OPTIONS_MESSAGE_TYPE_DISCOVER:
                    break;

                case DHCP_OPTIONS_MESSAGE_TYPE_OFFER:
                    break;

                case DHCP_OPTIONS_MESSAGE_TYPE_REQUEST:
                    break;

                case DHCP_OPTIONS_MESSAGE_TYPE_DECLINE:
                    break;

                case DHCP_OPTIONS_MESSAGE_TYPE_ACK:
                    /* Subnet Mask */
                    uint8_t *subnet_mask;
                    subnet_mask = dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_SUBNET_MASK);
                    memcpy(&dhcp_config.subnet_mask, subnet_mask, 4);

                    /* Router */
                    uint8_t *router;
                    router = dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_ROUTER);
                    memcpy(&dhcp_config.router, router, 4);

                    /* DNS */
                    uint8_t *dns;
                    dns = dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_DNS);
                    memcpy(&dhcp_config.dns, dns, 4);

                    /* IP */
                    memcpy(&dhcp_config.ip, &packet->yiaddr, 4);

                    kdebug("[dhcp] IP: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config.ip[i]);
                    }
                    kdebug("%d | ", dhcp_config.ip[3]);

                    kdebug("Subnet Mask: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config.subnet_mask[i]);
                    }
                    kdebug("%d | ", dhcp_config.subnet_mask[3]);

                    kdebug("DNS: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config.dns[i]);
                    }
                    kdebug("%d | ", dhcp_config.dns[3]);

                    kdebug("Router: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config.router[i]);
                    }
                    kdebug("%d\r\n", dhcp_config.router[3]);

                    break;
            }
        }
    }
}

void dhcp_discover()
{
    uint8_t dst_ip[4];
    memset(dst_ip, 0xFF, 4);

    dhcp_packet *packet = (dhcp_packet *) malloc(sizeof(dhcp_packet));
    memset(packet, 0x00, sizeof(dhcp_packet));

    packet->op = DHCP_PACKET_OP_REQUEST;
    packet->htype = DHCP_PACKET_HTYPE_ETHERNET;
    packet->hlen = 6; // MAC
    packet->hops = 0;
    packet->xid = 0x21274A1D;
    packet->secs = 0;
    packet->flags = DHCP_PACKET_FLAG_BROADCAST;

    memcpy(packet->chaddr, arp_get_mac(), 6);

    udp_send_packet(dst_ip, 68, 67, (uint8_t *) packet, sizeof(dhcp_packet));
}