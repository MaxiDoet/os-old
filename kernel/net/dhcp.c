#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/net/net.h"
#include "../include/kernel/net/dhcp.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../libc/include/string.h"
#include "../config.h"

#define DHCP_PACKET_MAGIC_COOKIE 0x63825363

#define DHCP_PACKET_OP_REQUEST  1
#define DHCP_PACKET_OP_REPLY    2

#define DHCP_PACKET_HTYPE_ETHERNET 1

#define DHCP_PACKET_FLAG_BROADCAST (1 << 15)

#define DHCP_OPTIONS_CODE_SUBNET_MASK   1
#define DHCP_OPTIONS_CODE_ROUTER        3
#define DHCP_OPTIONS_CODE_DNS           6
#define DHCP_OPTIONS_CODE_REQUESTED_IP  50
#define DHCP_OPTIONS_CODE_MESSAGE_TYPE  53
#define DHCP_OPTIONS_CODE_DHCP_SERVER   54
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
    uint8_t reserved[192];
    uint32_t magic_cookie;
    uint8_t options[312];
} __attribute__((packed)) dhcp_packet;

typedef struct dhcp_option_t {
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) dhcp_option_t;

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

uint8_t *dhcp_create_option(dhcp_option_t *options, uint32_t *offset, uint8_t type, uint8_t length)
{
    dhcp_option_t *option = (dhcp_option_t *) ((uint8_t *) options + *offset);

    option->type = type;
    option->length = length;

    *offset += sizeof(dhcp_option_t) + length;

    return ((uint8_t *) option + sizeof(dhcp_option_t));
}

void dhcp_request(uint8_t *ip)
{
    dhcp_config_t *dhcp_config = net_get_dhcp_config();

    #ifdef NET_DEBUG_DHCP
    kdebug("[net] DHCP | Sending request of ");
    net_print_ip(ip);
    kdebug(" to ");
    net_print_ip(dhcp_config->dhcp_server);
    kdebug("\r\n");
    #endif

    dhcp_packet *packet = (dhcp_packet *) malloc(sizeof(dhcp_packet) + 100);
    memset(packet, 0x00, sizeof(dhcp_packet));

    packet->op = DHCP_PACKET_OP_REQUEST;
    packet->htype = DHCP_PACKET_HTYPE_ETHERNET;
    packet->hlen = 6; // MAC
    packet->hops = 0;
    packet->xid = 0x21274A1D;
    packet->secs = 0;

    packet->magic_cookie = net_swap_dword(DHCP_PACKET_MAGIC_COOKIE);

    memcpy(packet->chaddr, arp_get_mac(), 6);

    dhcp_option_t *options = (dhcp_option_t *) packet->options;
    uint32_t offset = 0;

    /* Message type: Request */
    uint8_t *message_type = dhcp_create_option(options, &offset, DHCP_OPTIONS_CODE_MESSAGE_TYPE, 1);
    *message_type = DHCP_OPTIONS_MESSAGE_TYPE_REQUEST;

    /* Requested IP */
    uint8_t *requested_ip = dhcp_create_option(options, &offset, DHCP_OPTIONS_CODE_REQUESTED_IP, 4);
    memcpy(requested_ip, ip, 4);

    /* DHCP Server */
    uint8_t *dhcp_server = dhcp_create_option(options, &offset, DHCP_OPTIONS_CODE_DHCP_SERVER, 4);
    memcpy(dhcp_server, dhcp_config->dhcp_server, 4);

    /* End */
    dhcp_create_option(options, &offset, DHCP_OPTIONS_CODE_END, 1);

    udp_send_packet(dhcp_config->dhcp_server, 68, 67, (uint8_t *) packet, sizeof(dhcp_packet) + offset);
}

void dhcp_discover()
{
    uint8_t dst_ip[4];
    memset(dst_ip, 0xFF, 4);

    dhcp_packet *packet = (dhcp_packet *) malloc(sizeof(dhcp_packet) + 100);
    memset(packet, 0x00, sizeof(dhcp_packet));

    packet->op = DHCP_PACKET_OP_REQUEST;
    packet->htype = DHCP_PACKET_HTYPE_ETHERNET;
    packet->hlen = 6; // MAC
    packet->hops = 0;
    packet->xid = 0x21274A1D;
    packet->secs = 0;
    packet->flags = DHCP_PACKET_FLAG_BROADCAST;

    packet->magic_cookie = net_swap_dword(DHCP_PACKET_MAGIC_COOKIE);

    memcpy(packet->chaddr, arp_get_mac(), 6);

    dhcp_option_t *options = (dhcp_option_t *) packet->options;
    uint32_t offset = 0;

    /* Message type: Request */
    uint8_t *message_type = dhcp_create_option(options, &offset, DHCP_OPTIONS_CODE_MESSAGE_TYPE, 1);
    *message_type = DHCP_OPTIONS_MESSAGE_TYPE_DISCOVER;

    /* End */
    dhcp_create_option(options, &offset, DHCP_OPTIONS_CODE_END, 1);

    udp_send_packet(dst_ip, 68, 67, (uint8_t *) packet, sizeof(dhcp_packet) + offset);
}

void dhcp_handle_packet(uint8_t *data, uint32_t size)
{
    dhcp_packet *packet = (dhcp_packet *) data;
    dhcp_config_t *dhcp_config = net_get_dhcp_config();

    #ifdef NET_DEBUG_DHCP
    kdebug("[net] DHCP | ");
    #endif

    if (packet->op == DHCP_PACKET_OP_REQUEST) {
        #ifdef NET_DEBUG_DHCP
        kdebug("Request\r\n");
        #endif
    } else if (packet->op == DHCP_PACKET_OP_REPLY) {
        uint8_t message_type = 0;

        #ifdef NET_DEBUG_DHCP
        kdebug("Reply | ");
        #endif

        if (message_type = *dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_MESSAGE_TYPE)) {
            switch(message_type) {
                case DHCP_OPTIONS_MESSAGE_TYPE_OFFER:
                    #ifdef NET_DEBUG_DHCP
                    kdebug("OFFER\r\n");
                    #endif

                    /* Subnet Mask */
                    uint8_t *subnet_mask;
                    subnet_mask = dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_SUBNET_MASK);
                    memcpy(dhcp_config->subnet_mask, subnet_mask, 4);

                    /* Router */
                    uint8_t *router;
                    router = dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_ROUTER);
                    memcpy(dhcp_config->router, router, 4);

                    /* DNS */
                    uint8_t *dns;
                    dns = dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_DNS);
                    memcpy(dhcp_config->dns, dns, 4);

                    /* DHCP Server */
                    uint8_t *dhcp_server = dhcp_read_option((dhcp_option_t *) packet->options, DHCP_OPTIONS_CODE_DHCP_SERVER);
                    memcpy(dhcp_config->dhcp_server, dhcp_server, 4);

                    /* IP */
                    memcpy(dhcp_config->ip, &packet->yiaddr, 4);

                    //dhcp_request(dhcp_config->ip);
                    
                    break;

                case DHCP_OPTIONS_MESSAGE_TYPE_DECLINE:
                    #ifdef NET_DEBUG_DHCP
                    kdebug("DECLINE\r\n");
                    #endif
                    
                    break;

                case DHCP_OPTIONS_MESSAGE_TYPE_ACK:
                    #ifdef NET_DEBUG_DHCP
                    kdebug("ACK | ");

                    kdebug("IP: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config->ip[i]);
                    }
                    kdebug("%d | ", dhcp_config->ip[3]);

                    kdebug("Subnet Mask: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config->subnet_mask[i]);
                    }
                    kdebug("%d | ", dhcp_config->subnet_mask[3]);

                    kdebug("DNS: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config->dns[i]);
                    }
                    kdebug("%d | ", dhcp_config->dns[3]);

                    kdebug("Router: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config->router[i]);
                    }
                    kdebug("%d | ", dhcp_config->router[3]);

                    kdebug("DHCP: ");
                    for (uint8_t i=0; i < 3; i++) {
                        kdebug("%d.", dhcp_config->dhcp_server[i]);
                    }
                    kdebug("%d\r\n", dhcp_config->dhcp_server[3]);
                    #endif

                    dhcp_config->ack = true;

                    break;
            }
        }
    }
}