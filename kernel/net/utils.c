#include <stdint.h>
#include <stdbool.h>

#include "../libc/include/string.h"
#include "../include/kernel/net/utils.h"

uint8_t net_swap_byte(uint8_t byte) {
    return ((byte & 0x0F) << 4) | ((byte & 0xF0) >> 4);
}

uint16_t net_swap_word(uint16_t word)
{
    return (word & 0xFF00) >> 8 | (word & 0x00FF) << 8;
}