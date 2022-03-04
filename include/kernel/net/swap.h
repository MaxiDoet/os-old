#ifndef _net_swap_h
#define _net_swap_h

uint16_t net_swap_word(uint16_t word)
{
        return (word & 0xFF00) >> 8 | (word & 0x00FF) << 8;
}

#endif
