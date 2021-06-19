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
#include "../include/kernel/ethernet.h"
#include "../include/kernel/arp.h"

pci_dev_descriptor dev;
uint16_t *rx_buffer;
uint16_t rx_buffer_offset;
uint16_t *tx_buffers[4];
int tx_buffer_current;

void rtl8139_handle_rx();

void rtl8139_irq_handler()
{
	uint16_t int_mask = inw(dev.bars[0].io_base + REG_IMR);
	uint16_t int_status = inw(dev.bars[0].io_base + REG_ISR);
	kdebug("[rtl8139] IRQ! int_mask: %x int_status: %x\r\n", int_mask, int_status);

	bool tok = inw(dev.bars[0].io_base + REG_ISR) & (1 << 2); // Transmit ok
	bool rok = inw(dev.bars[0].io_base + REG_ISR) & (1 << 0); // Receive ok

	// Clear isr
	outw(dev.bars[0].io_base + REG_ISR, 0x1);

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

		for (int i=0; i < 100; i++) {
			kdebug("%x ", *(buffer + i));
		}


		// Check packet
		if (*buffer & (1 << 0)) {
			kdebug("rok! length: %x\r\n", length);

			etherframe_header *frame_header = (etherframe_header *) (buffer + 2);
			kdebug("ether_type: %x dst_mac: %x\r\n", frame_header->ether_type, frame_header->dst_mac);

			if (frame_header->ether_type == ETHERTYPE_ARP) {
				arp_packet *packet = (arp_packet *) ((uint32_t) frame_header + 14);

				kdebug("hw: %x nw: %x\r\n", packet->hardwareaddress_type, packet->networkaddress_type);
				kdebug("arp: hardwareaddress_type: %s networkaddress_type: %s\r\n", ((packet->hardwareaddress_type == 0x100) ? "MAC" : "Unknown"), ((packet->networkaddress_type == 0x8) ? "IPv4" : "Unknown"));
			}
		}
	}
}

void rtl8139_send(uint16_t *data, uint32_t len)
{
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
	uint8_t mac[6];
	kdebug("[rtl8139] MAC: ");
	for (int i=0; i < 6; i++) {
		mac[i] = inb(dev.bars[0].io_base + i);
		kdebug("%x%s", mac[i], ((i < 5) ? ":" : "\r\n"));
	}

	/*
	// Test packet
	tx_buffers[0] = (uint16_t *) 0x00030000;
	tx_buffers[1] = (uint16_t *) 0x00040000;
	tx_buffers[2] = (uint16_t *) 0x00050000;
	tx_buffers[3] = (uint16_t *) 0x00060000;

	memset(tx_buffers[0], 0x00, 1792);
	memset(tx_buffers[1], 0x00, 1792);
	memset(tx_buffers[2], 0x00, 1792);
	memset(tx_buffers[3], 0x00, 1792);

	etherframe_header header;
	header.dst_mac = 0xffffff;
	header.src_mac = 0x7636E89F646d;
	header.ether_type = 0x0806;

	rtl8139_send((uint16_t *) &header, sizeof(etherframe_header));
	*/
}
