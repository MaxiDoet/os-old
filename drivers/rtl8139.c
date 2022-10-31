#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/mem/heap.h"
#include "../include/kernel/io.h"
#include "../include/drivers/rtl8139.h"
#include "../include/drivers/pci.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/ip.h"

// Registers
#define REG_CR 0x37
#define REG_CAPR 0x38
#define REG_IMR 0x3C
#define REG_ISR 0x3E
#define REG_TCR 0x40
#define REG_RCR 0x44
#define REG_RBSTART 0x30

// Buffers
#define TX_BUFFER_SIZE 1792
#define RX_BUFFER_SIZE 9744

// Interrupt mask register masks
#define IMR_SERR (1 << 15)
#define IMR_TIMEOUT (1 << 14)
#define IMR_LENCHG (1 << 13)
#define IMR_FOVW (1 << 6)
#define IMR_PUN (1 << 5)
#define IMR_RXOVW (1 << 4)
#define IMR_TER (1 << 3)
#define IMR_TOK (1 << 2)
#define IMR_RER (1 << 1)
#define IMR_ROK (1 << 0)

// Transmit start address registers
#define REG_TSAD0 0x20
#define REG_TSAD1 0x24
#define REG_TSAD2 0x28
#define REG_TSAD3 0x2C

// Transmit status registers
#define REG_TSD0 0x10
#define REG_TSD1 0x14
#define REG_TSD2 0x18
#define REG_TSD3 0x1C

// Config registers
#define REG_CONFIG0 0x51
#define REG_CONFIG1 0x52
#define REG_CONFIG3 0x59
#define REG_CONFIG4 0x5A

// Command register
#define CR_RST (1 << 4)
#define CR_RE (1 << 3)
#define CR_TE (1 << 2)
#define CR_BUFE (1 << 0)

// RX config
#define RCR_ACCEPT_PHYSICAL_ADDRESS_PACKETS (1 << 0)
#define RCR_ACCEPT_PHYSICAL_MATCH_PACKETS (1 << 1)
#define RCR_ACCEPT_MULTICAST_PACKETS (1 << 2)
#define RCR_ACCEPT_BROADCAST_PACKETS (1 << 3)
#define RCR_ACCEPT_RUNT_PACKETS (1 << 4)
#define RCR_ACCEPT_ERROR_PACKETS (1 << 5)
#define RCR_WRAP (1 << 7)

pci_dev_descriptor dev;
uint16_t *rx_buffer;
uint16_t rx_buffer_offset;
uint16_t *tx_buffers[4];
uint8_t tx_buffer_current;

void rtl8139_handle_rx();
bool init_done = false;

void rtl8139_irq_handler()
{
	if (!init_done) {
		return;
	}

	uint16_t int_mask = inw(dev.bars[0].io_base + REG_IMR);
	uint16_t int_status = inw(dev.bars[0].io_base + REG_ISR);
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

		kdebug("[rtl8139] rx: ");
		for (int i=0; i < length; i++) {
			kdebug("%x ", *(buffer + i));
		}
		kdebug("\r\n");

		// Check packet
		if (*buffer & (1 << 0)) {
			ethernet_handle_frame(buffer, length);
		}
	}
}

void rtl8139_send(uint16_t *data, uint32_t len)
{
	kdebug("[rtl8139] tx (%d bytes): ", len);
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

	/* Enable bus mastering */
	uint16_t command = pci_read_word(dev.bus, dev.device, dev.function, PCI_REGISTER_COMMAND);
	command |= PCI_COMMAND_BUSMASTER;
	pci_write_word(dev.bus, dev.device, dev.function, PCI_REGISTER_COMMAND, command);

	// Setup rx buffer
	rx_buffer = (uint16_t *) malloc(8192 + 16 + 1500);
	outl(dev.bars[0].io_base + REG_RBSTART, (uint32_t) rx_buffer);
	memset(rx_buffer, 0x00, 8192 + 16);

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

	// Init TX buffers
	tx_buffers[0] = malloc(1792);
	tx_buffers[1] = malloc(1792);
	tx_buffers[2] = malloc(1792);
	tx_buffers[3] = malloc(1792);

	// Install irq handler
    irq_install_handler(dev.irq, rtl8139_irq_handler);

	// Read MAC
	uint8_t mac[6];
	kdebug("[rtl8139] MAC: ");
	for (int i=0; i < 6; i++) {
		mac[i] = inb(dev.bars[0].io_base + i);
		kdebug("%x%s", mac[i], ((i < 5) ? ":" : "\r\n"));
	}

	// Set static ip and mac
	uint8_t static_ip[4] = {10, 0, 2, 15};
	arp_set_ip(static_ip);
	arp_set_mac(mac);

	init_done = true;

	// Default qemu gateway
	uint8_t gateway_ip[4] = {10, 0, 2, 2};
	arp_request_mac(gateway_ip);
	//arp_broadcast_mac(gateway_ip);

	uint16_t status = inw(dev.bars[0].io_base + 0x64);
	kdebug("status: %x\r\n", status);
}
