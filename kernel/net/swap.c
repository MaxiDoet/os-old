#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/net/swap.h"

uint16_t net_swap_word(uint16_t word)
{
	return (word & 0xFF00) >> 8 | (word & 0x00FF) << 8;
}
