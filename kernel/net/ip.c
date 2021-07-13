#include <stdint.h>
#include <stdbool.h>

#include "../include/kernel/net/ip.h"
#include "../include/kernel/kernel.h"

void ip_handle_packet(ip_packet *packet)
{
	kdebug("[ip] version: %d protcol: %x\r\n", packet->version, packet->protocol);
}
