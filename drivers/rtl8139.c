#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/mem/heap.h"
#include "../include/kernel/io/io.h"
#include "../include/drivers/rtl8139.h"
#include "../include/kernel/io/pci.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/net/ethernet.h"
#include "../include/kernel/net/arp.h"
#include "../include/kernel/net/ip.h"

#define RTL8139_DEBUG

// Registers
#define REG_CR 0x37
#define REG_CAPR 0x38
#define REG_IMR 0x3C
#define REG_ISR 0x3E
#define REG_TCR 0x40
#define REG_RCR 0x44
#define REG_RBSTART 0x30

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

// Interrupt status register masks
#define ISR_ROK (1 << 0)
#define ISR_RER (1 << 1)
#define ISR_TOK (1 << 2)
#define ISR_TER	(1 << 3)
#define ISR_RBO (1 << 4)

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

pci_dev_t dev;
uint8_t *rx_buffer;
uint32_t rx_buffer_offset;
uint8_t *tx_buffer;
uint8_t tx_buffer_current;
bool init_done = false;

void rtl8139_handle_rx();

void rtl8139_irq_handler()
{
	if (!init_done) {
		return;
	}

	uint16_t imr = inw(dev.bars[0].io_base + REG_IMR);
	uint16_t isr = inw(dev.bars[0].io_base + REG_ISR);

	uint16_t isr_clear = 0;

	if (isr & ISR_RER) {
		kdebug("rx error\r\n");
	}

	if (isr & ISR_TER) {
		kdebug("tx error\r\n");
	}

	if (isr & ISR_RBO) {
		kdebug("rx buffer overflow\r\n");
	}

	if (isr & ISR_ROK) {
		rtl8139_handle_rx(dev);
		isr_clear |= ISR_ROK;
	}

	if (isr & ISR_TOK) {
		// Handle tx
		isr_clear |= ISR_TOK;
	}

	outw(dev.bars[0].io_base + REG_ISR, isr_clear);
}

void rtl8139_handle_rx()
{
	uint8_t cr = inb(dev.bars[0].io_base + REG_CR);

	kdebug("rx\r\n");

	if (cr & CR_BUFE) {
		#ifdef RTL8139_DEBUG
		kdebug("[rtl8139] empty rx ring buffer\r\n");
		#endif

		return;
	}

	uint8_t *buffer = (uint8_t *) (rx_buffer + rx_buffer_offset);
	uint16_t size = *((uint16_t *) (buffer + 2));

	uint8_t *packet = (uint8_t *) malloc(size);
	memcpy(packet, buffer, size);

	ethernet_handle_frame(packet + 4, size);

	rx_buffer_offset = (rx_buffer_offset + size + 4 + 3) & ~0x3;
	outw(dev.bars[0].io_base + REG_CAPR, rx_buffer_offset - 0x10);
}

void rtl8139_send(uint8_t *data, uint32_t size)
{
	// Copy data to current tx buffer
	memcpy(tx_buffer, data, size); 

	// If size is less than 60 we need to fill the rest with zeros
	if (size < 60) {
		memset((uint8_t *) tx_buffer + size, 0x00, 60 - size);
		size = 60;
	}

	uint8_t tx_buffer_current_tmp = tx_buffer_current;
	tx_buffer_current++;
	tx_buffer_current %= 4;

	// Send tx buffer address
	outl(dev.bars[0].io_base + 0x20 + (tx_buffer_current_tmp * 4), (uint32_t) tx_buffer);

	// Send size
	outl(dev.bars[0].io_base + 0x10 + (tx_buffer_current_tmp * 4), size);
}

void rtl8139_init(pci_dev_t pci_dev)
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
	rx_buffer = (uint8_t *) malloc(8192 + 16 + 1500);
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
	tx_buffer = malloc(1792);

	// Install irq handler
    irq_install_handler(dev.irq, rtl8139_irq_handler);

	// Read MAC
	uint8_t mac[6];
	kdebug("[rtl8139] MAC: ");
	for (int i=0; i < 6; i++) {
		mac[i] = inb(dev.bars[0].io_base + i);
		kdebug("%x%s", mac[i], ((i < 5) ? ":" : "\r\n"));
	}

	init_done = true;

	// Set static ip and mac
	uint8_t static_ip[4] = {10, 0, 2, 15};
	arp_set_ip(static_ip);
	arp_set_mac(mac);
}
