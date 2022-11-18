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

    memcpy(arp_get_mac(), packet->chaddr, 6);

    /*
    for (uint16_t i=0; i < sizeof(dhcp_packet); i++) {
        kdebug("%x ", *(packet + i));
    }
    */

    udp_send_packet(dst_ip, 68, 67, (uint8_t *) packet, sizeof(dhcp_packet));
}