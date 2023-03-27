#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/net/dns.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/net/net.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../config.h"

#define DNS_FLAGS_QR (1 << 0)
#define DNS_FLAGS_RD (1 << 8)

#define DNS_FLAGS_RCODE_NE              0
#define DNS_FLAGS_RCODE_FORMAT          1
#define DNS_FLAGS_RCODE_SERVER_FAILURE  2
#define DNS_FLAGS_RCODE_NAME_ERROR      3
#define DNS_FLAGS_RCODE_REFUSED         5

const char *dns_flags_rcode_names[] = {
    "",
    "Format Error",
    "Server failure",
    "Name Error",
    "",
    "Refused"
};

#define DNS_TYPE_A  1

#define DNS_CLASS_IN 1

typedef struct dns_header_t {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed)) dns_header_t;

typedef struct dns_question_t {
    uint16_t type;
    uint16_t class;
} __attribute__((packed)) dns_question_t;

void dns_handle_packet(uint8_t *data, uint32_t size)
{
    dns_header_t *header = (dns_header_t *) data;

    header->flags = net_swap_word(header->flags);

    uint8_t rcode = (header->flags & 0x000F);
    
    if (rcode) {
        kdebug("[net] DNS | Reply | %s\r\n", dns_flags_rcode_names[rcode]);
        return;
    }

    if (header->ancount) {
        
    }
}

void dns_convert_name(uint8_t *ptr, char *cname)
{
    uint8_t *ptr_tmp = ptr;
    char *cname_tmp = cname;
    int left = strlen(cname);
    int label_index = 0;
    
    while(left >= 0) {
        if (cname_tmp[label_index] == '.' || left == 0) {
            char *label_tmp = (char *) malloc(label_index + 1);
            memcpy(label_tmp, cname_tmp, label_index + 1);
            
            *ptr_tmp = label_index;
            ptr_tmp++;
            memcpy(ptr_tmp, label_tmp, label_index);
            ptr_tmp += label_index;
            
            cname_tmp+=label_index + 1;
            label_index=0;
        } else {
            label_index++;
        }
        
        left--;
    }
}

void dns_request(uint8_t *dns_server_ip, char *cname)
{
    #ifdef NET_DEBUG_DNS
    kdebug("[net] DNS | Resolving of \"%s\" with ", cname);
    net_print_ip(dns_server_ip);
    kdebug("\r\n");
    #endif

    dns_header_t *header = (dns_header_t *) malloc(sizeof(dns_header_t) + 2 + strlen(cname) + sizeof(dns_question_t));

    header->id = 0x3413;
    header->flags = DNS_FLAGS_RD;
    header->flags = net_swap_word(header->flags);

    header->qdcount = net_swap_word(1);

    uint8_t *name = (char *) ((uint8_t *) header + sizeof(dns_header_t));
    dns_convert_name(name, cname);

    dns_question_t *question = (dns_question_t *) ((uint8_t *) header + sizeof(dns_header_t) + 2 + strlen(cname));
    question->type = net_swap_word(DNS_TYPE_A);
    question->class = net_swap_word(DNS_CLASS_IN);

    dhcp_config_t *dhcp_config = net_get_dhcp_config();
    uint8_t src_ip[4];
    memcpy(src_ip, dhcp_config->ip, 4);

    udp_socket_t *socket = udp_create();
    udp_bind(socket, dns_server_ip, 53);
    udp_send(socket, (uint8_t *) header, sizeof(dns_header_t) + 2 + strlen(cname) + sizeof(dns_question_t));

    //free(header);
}