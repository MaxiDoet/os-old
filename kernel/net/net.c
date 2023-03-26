#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/net/net.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/dhcp.h"
#include "../include/kernel/net/dns.h"
#include "../include/drivers/rtl8139.h"

dhcp_config_t dhcp_config;

dhcp_config_t *net_get_dhcp_config()
{
	return &dhcp_config;
}

void net_init()
{
    /* Init NIC */
    pci_dev_t *list = (pci_dev_t *) malloc(sizeof(pci_dev_t)* 1024);
	uint16_t list_length = pci_get_device_by_id(list, 0x10EC, 0x8139);
	for (uint16_t i=0; i < list_length; i++) {
		rtl8139_init(list[i]);
	}

	uint8_t dns_server_ip[4] = {10, 0, 2, 3};

	dhcp_discover();

	dns_request(dns_server_ip, "nvidia.com");
}
