#ifndef _net_h
#define _net_h

#include "../include/kernel/net/dhcp.h"

dhcp_config_t *net_get_dhcp_config();

void net_init();

#endif