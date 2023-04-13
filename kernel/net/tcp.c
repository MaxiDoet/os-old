#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/net/ip.h"
#include "../include/kernel/net/tcp.h"
#include "../include/kernel/net/dhcp.h"
#include "../include/kernel/net/dns.h"
#include "../include/kernel/net/net.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/mem/heap.h"

#define TCP_FLAG_FIN (1 << 0)
#define TCP_FLAG_SYN (1 << 1)
#define TCP_FLAG_RST (1 << 2)
#define TCP_FLAG_PSH (1 << 3)
#define TCP_FLAG_ACK (1 << 4)
#define TCP_FLAG_URG (1 << 5)
#define TCP_FLAG_ECE (1 << 6)
#define TCP_FLAG_CWR (1 << 7)

typedef enum tcp_state_t {
    TCP_STATE_LISTEN = 0,
    TCP_STATE_SYN_SENT,
    TCP_STATE_SYN_RECEIVED,
    TCP_STATE_ESTABLISHED,
    TCP_STATE_FIN_WAIT_1,
    TCP_STATE_FIN_WAIT_2,
    TCP_STATE_CLOSE_WAIT,
    TCP_STATE_CLOSING,
    TCP_STATE_LAST_ACK,
    TCP_STATE_TIME_WAIT,
    TCP_STATE_CLOSED
} tcp_state_t;

uint8_t state = TCP_STATE_LISTEN;

uint32_t snd_una;
uint32_t snd_nxt;
uint32_t snd_wnd;
uint32_t snd_up;
uint32_t snd_wl1;
uint32_t snd_wl2;
uint32_t iss;

uint32_t rcv_nxt;
uint32_t rcv_wnd;
uint32_t rcv_up;
uint32_t irs;

typedef struct tcp_header_t {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t reserved : 4;
    uint8_t data_offset : 4;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_pointer;
    uint32_t options;
} __attribute__((packed)) tcp_header_t;

typedef struct tcp_pseudo_header_t {
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint16_t protocol;
    uint16_t tcp_length;
} __attribute__((packed)) tcp_pseudo_header_t;

void tcp_send_packet(uint8_t *dst_ip, uint16_t src_port, uint16_t dst_port, uint32_t seq, uint32_t ack, uint8_t flags, uint8_t *data, uint32_t size)
{
    dhcp_config_t *dhcp_config = net_get_dhcp_config();

    uint8_t *buffer = (uint8_t *) malloc(sizeof(tcp_pseudo_header_t) + sizeof(tcp_header_t) + size);
    tcp_pseudo_header_t *pseudo_header = (tcp_pseudo_header_t *) buffer;
    tcp_header_t *header = (tcp_header_t *) (buffer + sizeof(tcp_pseudo_header_t));

    memcpy(pseudo_header->src_ip, dhcp_config->ip, 4);
    memcpy(pseudo_header->dst_ip, dst_ip, 4);
    pseudo_header->protocol = net_swap_word(IP_PROTOCOL_TCP);
    pseudo_header->tcp_length = net_swap_word(sizeof(tcp_header_t) + size);

    header->src_port = net_swap_word(src_port);
    header->dst_port = net_swap_word(dst_port);
    header->seq = net_swap_dword(seq);
    header->ack = net_swap_dword(ack);
    header->data_offset = sizeof(tcp_header_t) / 4;
    header->flags = flags;
    header->window = 0xFFFF;

    memcpy((uint8_t *) header + sizeof(tcp_header_t), data, size);

    header->checksum = 0;
    header->checksum = net_calculate_checksum(buffer, sizeof(tcp_pseudo_header_t) + sizeof(tcp_header_t) + size);

    ip_send_packet(dhcp_config->ip, dst_ip, IP_PROTOCOL_TCP, (uint8_t *) header, sizeof(tcp_header_t) + size);
}

void tcp_handle_packet(uint8_t *src_ip, uint8_t *data, uint32_t size)
{
    tcp_header_t *header = (tcp_header_t *) data;

    header->src_port = net_swap_word(header->src_port);
    header->dst_port = net_swap_word(header->dst_port);
    header->seq = net_swap_dword(header->seq);
    header->ack = net_swap_dword(header->ack);

    /*
    switch(state) {
        case TCP_STATE_LISTEN:
            if (header->flags & TCP_FLAG_RST) {
                return;
            }

            if (header->flags & TCP_FLAG_ACK) {
                tcp_send_packet(src_ip, header->dst_port, header->src_port, header->ack, 0, TCP_FLAG_RST, 0, 0);
                return;
            }

            if (header->flags & TCP_FLAG_SYN) {
                rcv_nxt = header->seq + 1;
                irs = header->seq;
                iss = 100;

                tcp_send_packet(src_ip, header->dst_port, header->src_port, iss, rcv_nxt, TCP_FLAG_SYN | TCP_FLAG_ACK, 0, 0);

                snd_nxt = iss + 1;
                snd_una = iss;

                state = TCP_STATE_SYN_RECEIVED;
            }

            break;

        case TCP_STATE_SYN_RECEIVED:
            if (header->flags & TCP_FLAG_ACK) {
                state = TCP_STATE_ESTABLISHED;
            }

            break;

        case TCP_STATE_ESTABLISHED:
            if (header->flags & TCP_FLAG_ACK) {
                kdebug("data: %s\r\n", (char *) ((uint8_t *) header + sizeof(tcp_header_t)));

                char *text = "HTTP/1.1 200 OK Content-Type: text/html <html><body><p>Hello World</p></body></html>";

                tcp_send_packet(src_ip, header->dst_port, header->src_port, snd_nxt, rcv_nxt, TCP_FLAG_ACK, 0, 0);

                kdebug("size: %d\r\n", size - sizeof(tcp_header_t));

                rcv_nxt += size - sizeof(tcp_header_t);                
                tcp_send_packet(src_ip, header->dst_port, header->src_port, snd_nxt, rcv_nxt, TCP_FLAG_ACK, text, strlen(text));
            }

            break;
    }
    */

    if (state == TCP_STATE_CLOSED) {
        if (header->flags & TCP_FLAG_ACK) {
            tcp_send_packet(src_ip, header->dst_port, header->src_port, header->ack, 0, TCP_FLAG_RST, 0, 0);
        }

        return;
    } else if (state == TCP_STATE_LISTEN) {
        if (header->flags & TCP_FLAG_RST) {
            return;
        }

        if (header->flags & TCP_FLAG_ACK) {
            tcp_send_packet(src_ip, header->dst_port, header->src_port, header->ack, 0, TCP_FLAG_RST, 0, 0);

            return;
        }

        if (header->flags & TCP_FLAG_SYN) {
            rcv_nxt = header->seq + 1;
            irs = header->seq;
            
            iss = 100;
            tcp_send_packet(src_ip, header->dst_port, header->src_port, iss, rcv_nxt, TCP_FLAG_SYN | TCP_FLAG_ACK, 0, 0);

            snd_nxt = iss + 1;
            snd_una = iss;

            state = TCP_STATE_SYN_RECEIVED;
        }
    } else {
        if (header->flags & TCP_FLAG_SYN) {
            tcp_send_packet(src_ip, header->dst_port, header->src_port, snd_nxt, rcv_nxt, TCP_FLAG_ACK, 0, 0);
            return;
        }

        if (header->flags & TCP_FLAG_ACK) {
            if (state == TCP_STATE_SYN_RECEIVED) {
                state = TCP_STATE_ESTABLISHED;

                snd_wnd = header->window;
                snd_wl1 = header->seq;
                snd_wl2 = header->ack;
            } else if (state == TCP_STATE_ESTABLISHED) {
                snd_una = header->ack;
            }
        } else {
            return;
        }

        if (state == TCP_STATE_ESTABLISHED) {
            rcv_nxt = header->seq + size - sizeof(tcp_header_t);
            rcv_wnd -= size - sizeof(tcp_header_t);
            tcp_send_packet(src_ip, header->dst_port, header->src_port, snd_nxt, rcv_nxt, TCP_FLAG_ACK, 0, 0);

            uint8_t *data = ((uint8_t *) header + sizeof(tcp_header_t));

            kdebug("data: %s\r\n", (char *) data);
        }
    }
}