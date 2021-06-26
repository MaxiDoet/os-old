#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/include/mm.h"
#include "../include/kernel/io.h"
#include "../include/drivers/rtl8139.h"
#include "../include/drivers/pci.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/kernel/net/arp.h"

pci_dev_descriptor dev;
uint16_t *rx_buffer;
uint16_t rx_buffer_offset;
uint16_t *tx_buffers[4];
int tx_buffer_current;

void rtl8139_handle_rx();
void rtl8139_send(uint16_t *data, uint32_t len);
bool init_done = false;

/* Some networking test code */
uint8_t mac[6];
uint8_t ip[4];
uint8_t mac_cache[100][6];
uint8_t ip_cache[100][4];
uint8_t cache_index;

void send_frame(uint8_t dst_mac[6], ethertype type, uint16_t *data, uint16_t size)
{
	uint16_t *buf = (uint16_t *) malloc(mm, sizeof(etherframe_header) + sizeof(arp_packet));
	memset(buf, 0x00, sizeof(etherframe_header) + size);

	etherframe_header *header = (etherframe_header *) buf;
	memcpy(header->dst_mac, &dst_mac, 6);
	memcpy(header->src_mac, &mac, 6);
	header->ether_type = type;

	memcpy(buf + sizeof(etherframe_header) - 7, data, size);

	rtl8139_send(buf, sizeof(etherframe_header) + size);
}

void request_mac(uint8_t ip[4])
{
	arp_packet *packet = (arp_packet *) malloc(mm, sizeof(arp_packet));
	memset((uint16_t *) packet, 0x00, sizeof(arp_packet));
	packet->hardwareaddress_type = 0x0100;
	packet->protocol_type = 0x0008;
	packet->hardwareaddress_length = 6;
	packet->networkaddress_length = 4;
	packet->operation_type = 0x0100;

	memcpy(packet->src_mac, &mac, 6);
	memcpy(packet->src_ip, &ip, 4);
	memset(packet->dst_mac, 0xFF, 6);
	memcpy(packet->dst_ip, ip, 4);

	uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	send_frame(broadcast_mac, ETHERTYPE_ARP, (uint16_t *) packet, sizeof(arp_packet));
}

void rtl8139_irq_handler()
{
	if (!init_done) {
		return;
	}

	uint16_t int_mask = inw(dev.bars[0].io_base + REG_IMR);
	uint16_t int_status = inw(dev.bars[0].io_base + REG_ISR);
	kdebug("[rtl8139] IRQ! int_mask: %x int_status: %x\r\n", int_mask, int_status);

	bool tok = inw(dev.bars[0].io_base + REG_ISR) & (1 << 2); // Transmit ok
	bool rok = inw(dev.bars[0].io_base + REG_ISR) & (1 << 0); // Receive ok

	// Clear isr
	outw(dev.bars[0].io_base + REG_ISR, (1 << 0) | (1 << 2));

	if (rok) {
		rtl8139_handle_rx(dev);
	}

	if (tok) {
		// Handle tx
	}
}

void rtl8139_handle_rx()
{
	while (!(inb(dev.bars[0].io_base + REG_CR) & CR_BUFE)) {
		uint16_t *buffer = (uint16_t *) ((uint32_t) rx_buffer + rx_buffer_offset);
		uint16_t length = *(buffer + 1);

		rx_buffer_offset += length + 4;
		rx_buffer_offset = (rx_buffer_offset + 3) & ~0x3;
		outw(dev.bars[0].io_base + REG_CAPR, rx_buffer_offset - 0x10);

		for (int i=0; i < length; i++) {
			kdebug("%x ", *(buffer + i));
		}


		// Check packet
		if (*buffer & (1 << 0)) {
			kdebug("rok! length: %x\r\n", length);

			etherframe_header *frame_header = (etherframe_header *) (buffer + 2);
			kdebug("ether_type: %x dst_mac: ", frame_header->ether_type);

			for (int i=0; i < 6; i++) {
				kdebug("%x%s", frame_header->dst_mac[i], ((i < 5) ? ":" : " src_mac: "));
			}

			for (int i=0; i < 6; i++) {
                                kdebug("%x%s", frame_header->src_mac[i], ((i < 5) ? ":" : "\r\n"));
                        }

			if (frame_header->ether_type == ETHERTYPE_ARP) {
				arp_packet *packet = (arp_packet *) ((uint32_t) frame_header + sizeof(etherframe_header));

				// ARP request
				if (packet->operation_type == 0x0100) {
					kdebug("arp request: hardwareaddress_type: %s protocol_type: %s\r\n", ((packet->hardwareaddress_type == 0x100) ? "MAC" : "Unknown"), ((packet->protocol_type == 0x8) ? "IPv4" : "Unknown"));

					/*
					// Send reply
					uint16_t *buf = (uint16_t *) malloc(mm, sizeof(etherframe_header) + sizeof(arp_packet));
					memset(buf, 0x00, sizeof(etherframe_header) + sizeof(arp_packet));

					etherframe_header *header = (etherframe_header *) buf;
					memset(header->dst_mac, 0xFF, 6);
					memcpy(header->src_mac, &mac, 6);
					header->ether_type = ETHERTYPE_ARP;
					*/

					arp_packet *reply_packet = (arp_packet *) malloc(mm, sizeof(arp_packet));
					reply_packet->hardwareaddress_type = 0x0100;
					reply_packet->protocol_type = 0x0008;
					reply_packet->hardwareaddress_length = 6;
					reply_packet->networkaddress_length = 4;
					reply_packet->operation_type = 0x0200;

					memcpy(reply_packet->src_mac, &mac, 6);
					memcpy(reply_packet->src_ip, &ip, 4);
					memcpy(reply_packet->dst_mac, packet->src_mac, 6);
					memcpy(reply_packet->dst_ip, packet->src_ip, 4);

					uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
					send_frame(broadcast_mac, ETHERTYPE_ARP, (uint16_t *) reply_packet, sizeof(arp_packet));

				} else if (packet->operation_type == 0x0200) {
					kdebug("arp reply: ip: ");

					for (int i=0; i < 4; i++) {
						kdebug("%d%s", packet->src_ip[i], ((i < 3) ? "." : " mac: "));
					}

					for (int i=0; i < 6; i++) {
                                                kdebug("%x%s", packet->src_mac[i], ((i < 5) ? ":" : "\r\n"));
                                        }

					// ARP reply
					memcpy(&mac_cache[cache_index], packet->src_mac, 6);
					memcpy(&ip_cache[cache_index], packet->src_ip, 4);
					cache_index++;
				}
			}
		}
	}
}

void rtl8139_send(uint16_t *data, uint32_t len)
{
	kdebug("Sending packet (%d bytes):\r\n", len);
	for (int i=0; i < len; i++) {
		kdebug("%x ", *(data + i));
	}
	kdebug("\r\n");

	// Copy data to current tx buffer
	memcpy(tx_buffers[tx_buffer_current], data, len);

	// Send tx buffer address
	outl(dev.bars[0].io_base + 0x20 + (tx_buffer_current * 4), (uint32_t) tx_buffers[tx_buffer_current]);

	// Send size
	outl(dev.bars[0].io_base + 0x10 + (tx_buffer_current * 4), len);

	tx_buffer_current++;
	if (tx_buffer_current > 3) tx_buffer_current = 0;
}

void rtl8139_init(pci_dev_descriptor pci_dev)
{
	dev = pci_dev;

	tx_buffer_current = 0;

	// Power on
	outb(dev.bars[0].io_base + REG_CONFIG1, 0x00);

	// Reset and wait
	outb(dev.bars[0].io_base + REG_CR, CR_RST);
	while((inb(dev.bars[0].io_base + REG_CR) & CR_RST) != 0) {
		// Wait until reset is done
	}

	pci_enable_bus_mastering(dev);

	// Setup rx buffer
	rx_buffer = (uint16_t *) 0x00070000;
	outl(dev.bars[0].io_base + REG_RBSTART, (uint32_t) rx_buffer);
	memset(rx_buffer, 0x00, 8192 + 16 + 1500);

	// Enable rx, tx
	outb(dev.bars[0].io_base + REG_CR, CR_RE | CR_TE);

	// RX Configuration
	outl(dev.bars[0].io_base + REG_RCR, RCR_ACCEPT_PHYSICAL_ADDRESS_PACKETS |
				    RCR_ACCEPT_PHYSICAL_MATCH_PACKETS   |
				    RCR_ACCEPT_MULTICAST_PACKETS        |
				    RCR_ACCEPT_BROADCAST_PACKETS        |
				    RCR_WRAP                            |
				    (6 << 8) // Max DMA burst size (1024 bytes)
	);

	// Setup IMR and ISR
	outw(dev.bars[0].io_base + REG_ISR, 0);
	outw(dev.bars[0].io_base + REG_IMR, 0xFFFF);

	// Install irq handler
        irq_install_handler(dev.irq, rtl8139_irq_handler);

	// Read MAC
	kdebug("[rtl8139] MAC: ");
	for (int i=0; i < 6; i++) {
		mac[i] = inb(dev.bars[0].io_base + i);
		kdebug("%x%s", mac[i], ((i < 5) ? ":" : "\r\n"));
	}

	// Set static ip
	ip[0] = 10;
	ip[1] = 0;
	ip[2] = 2;
	ip[3] = 14;

	// Test packet
	tx_buffers[0] = (uint16_t *) 0x00030000;
	tx_buffers[1] = (uint16_t *) 0x00040000;
	tx_buffers[2] = (uint16_t *) 0x00050000;
	tx_buffers[3] = (uint16_t *) 0x00060000;

	memset(tx_buffers[0], 0x00, 1792);
	memset(tx_buffers[1], 0x00, 1792);
	memset(tx_buffers[2], 0x00, 1792);
	memset(tx_buffers[3], 0x00, 1792);

	init_done = true;

	// Default qemu gateway
	uint8_t gateway_ip[4] = {10, 0, 2, 2};
	request_mac(gateway_ip);

	/*
	// Broadcast arp
	kdebug("broadcast arp\r\n");

	arp_packet *reply_packet = (arp_packet *) malloc(mm, sizeof(arp_packet));
	reply_packet->hardwareaddress_type = 0x0100;
	reply_packet->protocol_type = 0x0008;
	reply_packet->hardwareaddress_length = 6;
	reply_packet->networkaddress_length = 4;
	reply_packet->operation_type = 0x0200;

	memcpy(reply_packet->src_mac, &mac, 6);
	memcpy(reply_packet->src_ip, &ip, 4);
	memcpy(reply_packet->dst_mac, packet->src_mac, 6);
	memcpy(reply_packet->dst_ip, packet->src_ip, 4);

	for (int i=0; i < sizeof(etherframe_header) + sizeof(arp_packet); i++) {
		kdebug("%x ", *(buf + i));
	}

	rtl8139_send(buf, sizeof(etherframe_header) + sizeof(arp_packet));
	*/
}
