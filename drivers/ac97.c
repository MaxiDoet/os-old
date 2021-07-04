#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pci.h"
#include "../include/kernel/io.h"
#include "../include/drivers/ac97.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/vfs.h"
#include "../libc/include/mm.h"

pci_dev_descriptor dev;

struct buf_desc {
	uint32_t addr;
	uint16_t length;
 	int reserved : 14;
	unsigned int bup : 1;
	unsigned int ioc : 1;
} __attribute__((packed));

struct buf_desc buf_descriptors[32];

void ac97_irq_handler()
{
	kdebug("[ac97] irq!\r\n");
}

void ac97_reset(pci_dev_descriptor pci_dev)
{
	// Reset
	outl(pci_dev.bars[1].io_base + NABM_GLOBAL_CTL, 0x3);
	outw(pci_dev.bars[0].io_base + NAM_RESET, 0);

	// Set master volume
	outw(pci_dev.bars[0].io_base + NAM_MASTER_VOL, 0);
	// Set PCM output volume
	outw(pci_dev.bars[0].io_base + NAM_PCM_VOL, 0);

	// Reset output channel
	outb(pci_dev.bars[1].io_base + NABM_PCM_OUTPUT + CHANNEL_BUFFER_CNT, 0x2);

	while(inb(pci_dev.bars[1].io_base + NABM_PCM_OUTPUT + CHANNEL_BUFFER_CNT) == 0x2) {
	}

	kdebug("[ac97] reset done\r\n");
}

void ac97_init(pci_dev_descriptor pci_dev)
{
	dev = pci_dev;

	// Enable interrupts
	irq_install_handler(dev.irq, ac97_irq_handler);

	pci_enable_bus_mastering(dev);

	ac97_reset(dev);

	int i=0;
	int current_descriptor;
	bool running = true;
	for (int i=0; i < 10; i++) {
		//buf_descriptors[i].addr = (uint32_t) &audio_bin[i * 256000];
		buf_descriptors[i].length = 0xFFFE;
		buf_descriptors[i].ioc = 0;

		if (i == 9) {
			buf_descriptors[i].ioc = 1;
		}
	}

	outl(dev.bars[1].io_base + NABM_PCM_OUTPUT + CHANNEL_BUFFER_DSC_ADDR, (uint32_t) &buf_descriptors);
	outb(dev.bars[1].io_base + NABM_PCM_OUTPUT + CHANNEL_LAST_VALID_ENTRY, 10);
	outb(dev.bars[1].io_base + NABM_PCM_OUTPUT + CHANNEL_BUFFER_CNT, 0x1);
}
