#ifndef _ethernet_h
#define _ethernet_h

enum ethertype {
	ETHERTYPE_IP4 = 0x800,
	ETHERTYPE_ARP = 0x608
};

typedef struct etherframe_header {
        uint64_t dst_mac : 48;
        uint64_t src_mac : 48;
        uint16_t ether_type;
} __attribute__((packed)) etherframe_header;

#endif
