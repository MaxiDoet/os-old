#include <stdint.h>
#include <stdbool.h>

#include "../libc/include/string.h"
#include "../include/kernel/net/utils.h"
#include "../include/kernel/kernel.h"

void net_print_ip(uint8_t *ip)
{
    for (uint8_t i=0; i < 3; i++) {
        kdebug("%d.", ip[i]);
    }
    kdebug("%d", ip[3]);
}

void net_print_mac(uint8_t *mac)
{
    for (uint8_t i=0; i < 5; i++) {
        kdebug("%x:", mac[i]);
    }
    kdebug("%x", mac[5]);
}

uint8_t net_swap_byte(uint8_t byte) {
    return ((byte & 0x0F) << 4) | ((byte & 0xF0) >> 4);
}

uint16_t net_swap_word(uint16_t word)
{
    return (word & 0xFF00) >> 8 | (word & 0x00FF) << 8;
}

uint32_t net_swap_dword(uint32_t dword)
{
    return ((dword & 0xFF000000) >> 24) | ((dword & 0x00FF0000) >> 8) | ((dword & 0x0000FF00) << 8) | ((dword & 0x000000FF) << 24);
}

uint16_t net_calculate_checksum(uint8_t *data, uint32_t size)
{
    uint16_t *p = (uint16_t *)data;
    uint32_t sum = 0;

    while (size > 1)
    {
        sum += *p++;
        size -= 2;
    }

    if (size)
    {
        sum += *(uint8_t *)p;
    }

    sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);

    uint16_t temp = ~sum;

    return temp;
}