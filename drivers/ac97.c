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

#include "../include/drivers/audio.h"

static pci_dev_descriptor dev;

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
	outl(pci_dev.bars[1].io_base + GLOB_CNT, COLD_RESET);
	outw(pci_dev.bars[0].io_base + NAM_RESET, 0);

	// Reset output channel registers
	outb(pci_dev.bars[1].io_base + PO + CR, RR);

	while(inb(pci_dev.bars[1].io_base + PO + CR) == RR) {
	}

	kdebug("[ac97] reset done\r\n");
}

void ac97_play(uint8_t *data, uint32_t size) {
	uint32_t available = size;
	int last = 0;

	for (int i=0; i < 32; i++) {
		buf_descriptors[i].addr = (uint32_t) &data[i * 0xFFFF];
		buf_descriptors[i].ioc = 1;

		kdebug("[ac97] desc: %d available: %x\r\n", i, available);

		if (available >= 0x20000) {
			buf_descriptors[i].length = 0xFFFE;
			buf_descriptors[i].bup = 0;
			available -= 0x20000;
		} else {
			buf_descriptors[i].length = available >> 1;
			buf_descriptors[i].bup = 1;
			available = 0;
			last = i;
			kdebug("[ac97] last: %d\r\n", last);
			break;
		}
	}

	outl(dev.bars[1].io_base + PO + BDBAR, (uint32_t) buf_descriptors);
	outb(dev.bars[1].io_base + PO + LVI, last);

	// Start playback
	outb(dev.bars[1].io_base + PO + CR, RPBM);
}

void ac97_init(pci_dev_descriptor pci_dev)
{
	dev = pci_dev;
	pci_enable_bus_mastering(pci_dev);

	ac97_reset(pci_dev);

	// Enable interrupts
	irq_install_handler(dev.irq, ac97_irq_handler);
	outl(dev.bars[1].io_base + GLOB_CNT, GIE);
	outb(dev.bars[1].io_base + PO + CR, IOCE); // Interrupt on completion

	// Set master volume
	outw(pci_dev.bars[0].io_base + NAM_MASTER_VOL, 0x0000);
	// Set PCM output volume
	outw(pci_dev.bars[0].io_base + NAM_PCM_VOL, 0x0000);

	ac97_play(audio_wav, audio_wav_len);
}
