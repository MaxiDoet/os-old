#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/net/dns.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../config.h"

#define DNS_FLAGS_QR (1 << 0)

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
    if (!rcode) return;

    kdebug("[net] DNS | Reply | %s\r\n", dns_flags_rcode_names[rcode]);

    if (header->ancount) {
        
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

    header->qdcount = net_swap_word(1);

    uint8_t *name_length = ((uint8_t *) header + sizeof(dns_header_t));
    *name_length = strlen(cname);

    char *name = (char *) ((uint8_t *) header + sizeof(dns_header_t) + 1);
    memcpy(name, cname, strlen(cname));
    name[strlen(cname)] = 0x00;

    dns_question_t *question = (dns_question_t *) ((uint8_t *) header + sizeof(dns_header_t) + 2 + strlen(cname));
    question->type = net_swap_word(DNS_TYPE_A);
    question->class = net_swap_word(DNS_CLASS_IN);

    udp_send_packet(dns_server_ip, 0, 53, (uint8_t *) header, sizeof(dns_header_t) + 2 + strlen(cname) + sizeof(dns_question_t));
}