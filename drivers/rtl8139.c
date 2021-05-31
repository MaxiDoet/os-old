#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/include/mm.h"
#include "../include/kernel/asm.h"
#include "../include/drivers/rtl8139.h"
#include "../include/drivers/pci.h"
#include "../include/kernel/kernel.h"
#include "../libc/include/string.h"
#include "../include/kernel/irq.h"

pci_dev_descriptor dev;

void rtl8139_irq_handler()
{
	kdebug("[rtl8139] IRQ!\r\n");

	// Clear isr (only tx)
	outw(dev.io_base + 0x3E, 0x4);
}

void rtl8139_init(pci_dev_descriptor pci_dev)
{
	dev = pci_dev;

	// We need to activate PCI Bus Mastering by enabling it in the command register
	// Power on
	outb(dev.io_base + 0x52, 0x00);

	// Read MAC
	uint8_t mac[6];
	kdebug("[rtl8139] MAC: ");
	for (int i=0; i < 6; i++) {
		mac[i] = inb(dev.io_base + i);
		kdebug("%x%s", mac[i], ((i < 5) ? ":" : "\r\n"));
	}

	// Reset and wait
	outb(dev.io_base + 0x37, 0x10);
	while((inb(dev.io_base + 0x37) & 0x10) != 0);

	// Install irq handler
	irq_install_handler(dev.irq, rtl8139_irq_handler);

	// Allocate buffers
	uint16_t *rx_buffer = (uint16_t *) malloc(mm, 8208); // 8K + 16B
	uint16_t *tx_buffer = (uint16_t *) malloc(mm, 1792);

	// Send buffer addresses
	outl(dev.io_base + 0x30, (uintptr_t) rx_buffer);

	// Setup IMR and ISR
	outw(dev.io_base + 0x3C, 0x0005); // Only fire irq on TOK and ROK

	// Configure RCR
	outl(dev.io_base + 0x44, 0xF | (1 << 7));

	// Enable rx, tx
	outb(dev.io_base + 0x37, 0x0C);

	// Test packet
	memset(tx_buffer, 0x1, 1792);

	// First transmit buffer address
	outl(dev.io_base + 0x20, (uintptr_t) tx_buffer);

	// First transmit status
	outl(dev.io_base + 0x10, 1792);
}
