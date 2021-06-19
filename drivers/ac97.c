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
	unsigned int reserved : 14;
 	unsigned int bup : 1;
 	unsigned int ioc : 1;
} __attribute__((packed));

struct buf_desc buf_descriptors[8];

void ac97_irq_handler()
{
	kdebug("[ac97] irq!\r\n");
}

void ac97_init(pci_dev_descriptor pci_dev)
{
	dev = pci_dev;

	irq_install_handler(dev.irq, ac97_irq_handler);

	// Enable bus mastering
	uint16_t command = pci_read_word(dev.bus, dev.device, dev.function, 0x04);
	if (!(command & (1 << 2))) {
                command |= (1 << 2);
                pci_write_word(dev.bus, dev.device, dev.function, 0x04, command);
                kdebug("[ac97] enabled pci bus mastering\r\n");
        }

	// Reset
	outl(dev.bars[1].io_base + NABM_GLOBAL_CTL, 0x3);
	outw(dev.bars[0].io_base + NAM_RESET, 0);

	// Read card capabilities
	uint32_t status = inl(dev.bars[1].io_base + NABM_GLOBAL_STS);
	kdebug("Status: %x\r\n", status);

	// Set master volume
	outw(dev.bars[0].io_base + NAM_MASTER_VOL, 0);
	// Set PCM output volume
	outw(dev.bars[0].io_base + NAM_PCM_VOL, 0);

	// Reset output channel
	outb(dev.bars[1].io_base + 0x1B, 0x2);

	while(inb(dev.bars[1].io_base + 0x1B) == 0x2) {
		kdebug("Waiting for reset!\r\n");
	}

	kdebug("Reset done!\r\n");

	uint16_t *file_buf = (uint16_t *) malloc(mm, 10000);
        vfs_read("/audio.bin", file_buf);

	for (int j=0; j < 10000; j++) {
		kdebug("%x ", file_buf[j]);
	}

	int final;
	for (int i = 0; (i < 8); i++) {
		buf_descriptors[i].addr = (uint32_t) &file_buf;
		buf_descriptors[i].ioc = 1;
		buf_descriptors[i].length = 1000;
	}

	outl(dev.bars[1].io_base + 0x10, (uint32_t) &buf_descriptors);
	outb(dev.bars[1].io_base + 0x15, 8);
	outb(dev.bars[1].io_base + 0x1B, 0x1);
}
