#ifndef _dhcp_h
#define _dhcp_h

#define DHCP_PACKET_OP_REQUEST  1
#define DHCP_PACKET_OP_REPLY    2

#define DHCP_PACKET_HTYPE_ETHERNET 1

#define DHCP_PACKET_FLAG_BROADCAST (1 << 15)

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
    uint8_t chaddr[6];
    char sname[64];
    char file[128];
    uint8_t options[312];
} __attribute__((packed)) dhcp_packet;

void dhcp_discover();

#endif