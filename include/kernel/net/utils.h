#ifndef _net_swap_h
#define _net_swap_h

void net_print_ip(uint8_t *ip);
void net_print_mac(uint8_t *mac);
uint8_t net_swap_byte(uint8_t byte);
uint16_t net_swap_word(uint16_t word);
uint32_t net_swap_dword(uint32_t dword);

#endif
