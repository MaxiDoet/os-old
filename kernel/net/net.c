#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/net/net.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/udp.h"
#include "../include/kernel/net/dhcp.h"
#include "../include/kernel/net/dns.h"
#include "../include/drivers/rtl8139.h"

dhcp_config_t dhcp_config;

dhcp_config_t *net_get_dhcp_config()
{
	return &dhcp_config;
}

void test(uint8_t *data, uint32_t size)
{
	kdebug("Test: %s\r\n", (char *) data);
}

void net_init()
{
    /* Init NIC */
    pci_dev_t *list = (pci_dev_t *) malloc(sizeof(pci_dev_t)* 1024);
	uint16_t list_length = pci_get_device_by_id(list, 0x10EC, 0x8139);
	for (uint16_t i=0; i < list_length; i++) {
		rtl8139_init(list[i]);
	}

	dhcp_discover();

	/* Wait until DHCP is ready */
	while (!dhcp_config.ack) {

	}

	dns_request(dhcp_config.dns, "nvidia.com");
}
