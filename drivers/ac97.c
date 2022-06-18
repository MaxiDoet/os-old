#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/pci.h"
#include "../include/kernel/io.h"
#include "../include/drivers/ac97.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/kernel/fs/vfs.h"
#include "../libc/include/mm.h"

/* NAM Registers */
#define NAM_RESET 0x00
#define NAM_MASTER_VOL 0x02
#define NAM_MIC_VOL 0x0E
#define NAM_PCM_VOL 0x18
#define NAM_REC_SLC 0x1A
#define NAM_REC_GAIN 0x1C
#define NAM_MIC_GAIN 0x1E
#define NAM_EXT_ID 0x28
#define NAM_EXT_CTRL 0x2A
#define NAM_EXT_FRONT_RATE 0x2C

/* NABM Registers */
#define PI 0x00
#define PO 0x10
#define MIC 0x20
#define GLOB_CNT 0x2C
#define GLOB_STA 0x30

/* Channel Control Registers */
#define BDBAR 0x00
#define CIV 0x04
#define LVI 0x05
#define SR 0x06
#define PICB 0x08
#define PIV 0x0A
#define CR 0x0B

/* CR */
#define RPBM (1 << 0)
#define RR (1 << 1)
#define LVBIE (1 << 2)
#define IOCE (1 << 3)
#define FEIE (1 << 4)

/* SR */
#define DMAS (1 << 0)
#define EOT (1 << 1)
#define LBI (1 << 2)
#define IOCI (1 << 3)
#define FEI (1 << 4)

/* GLOB_CNT */
#define GIE (1 << 0)
#define COLD_RESET (1 << 1)
#define WARM_RESET (1 << 2)

#define MAX_ENTRY_SIZE 0xFFFE
#define LAST_VALID_INDEX 32

typedef struct buf_desc {
	uint32_t addr;
	uint16_t length;
 	int reserved : 14;
	unsigned int bup : 1;
	unsigned int ioc : 1;
} __attribute__((packed)) buf_desc;

static pci_dev_descriptor dev;
struct buf_desc buf_descriptors[32];
static uint8_t buf_descriptors_rp;
static uint8_t buf_descriptors_wp;

void ac97_irq_handler()
{
	uint16_t status = inw(dev.bars[1].io_base + PO + SR);

	if (status & LBI) {
		#ifdef AC97_DEBUG
		kdebug("Last Buffer Entry interrupt\r\n");
		#endif

		buf_descriptors_rp = 0;
		buf_descriptors_wp = 0;
	}

	if (status & IOCI) {
		#ifdef AC97_DEBUG
		kdebug("IOC interrupt\r\n");
		#endif

		buf_descriptors_rp = (buf_descriptors_rp + 1) % 32;

		#ifdef AC97_DEBUG
		kdebug("buf_descriptors_index: %d\r\n", buf_descriptors_rp);
		#endif
	}

	if (status & FEI) {
		#ifdef AC97_DEBUG
		kdebug("FIFO error interrupt\r\n");
		#endif
	}

	outw(dev.bars[1].io_base + PO + SR, 0x1C);
}

void ac97_reset(pci_dev_descriptor pci_dev)
{
	// Enable interrupts
	irq_install_handler(dev.irq, ac97_irq_handler);
	
	// Cold reset		
	outl(pci_dev.bars[1].io_base + GLOB_CNT, COLD_RESET);

	// Enable interrupts
	outl(pci_dev.bars[1].io_base + GLOB_CNT, GIE);

	// Reset mixer
	outw(pci_dev.bars[0].io_base + NAM_RESET, 1);

	// Reset output channel registers
	outb(pci_dev.bars[1].io_base + PO + CR, RR);
	while(inb(pci_dev.bars[1].io_base + PO + CR) & RR) {
	}

	kdebug("[ac97] reset done\r\n");
}

void ac97_write_single_buffer(uint8_t *data, uint16_t size)
{
	if (size > MAX_ENTRY_SIZE) return;

	buf_desc *desc = &buf_descriptors[buf_descriptors_wp];

	desc->addr = (uint32_t) data;
	desc->length = size;
	desc->ioc = 1;

	if (buf_descriptors_wp == LAST_VALID_INDEX) desc->bup = 1;

	buf_descriptors_wp = (buf_descriptors_wp + 1) % LAST_VALID_INDEX;
}

void ac97_play(uint8_t *data, uint32_t size)
{
	uint32_t available = size;
	uint32_t offset = 0;
	uint8_t last = 0;

	while (available) {
		uint16_t status = inw(dev.bars[1].io_base + PO + SR);
		uint8_t control = inb(dev.bars[1].io_base + PO + CR);

		if (status & DMAS) {
			// Refill buffers
			for (int i=0; i < LAST_VALID_INDEX; i++) {
				if (available >= 0x20000) {
					ac97_write_single_buffer(&data[offset], 0xFFFE);
					available -= 0x20000;
					offset += 0x20000;
					last = 32;
				} else {
					ac97_write_single_buffer(&data[offset], available >> 1);
					last = i;
					available = 0;
					break;
				}
			}

			// Reset output channel registers
			outb(dev.bars[1].io_base + PO + CR, RR);
			while(inb(dev.bars[1].io_base + PO + CR) & RR) {
			}

			outl(dev.bars[1].io_base + PO + BDBAR, (uint32_t) buf_descriptors);
			outb(dev.bars[1].io_base + PO + LVI, last - 1);

			// Start playback
			outb(dev.bars[1].io_base + PO + CR, RPBM | IOCE | LVBIE | FEIE); // Start DMA; Enable IOC interrupt; Enable Last Buffer Entry interrupt
		}
	}
}

void ac97_init(pci_dev_descriptor pci_dev)
{
	dev = pci_dev;

	/* Enable bus mastering */
	uint16_t command = pci_read_word(dev.bus, dev.device, dev.function, PCI_REGISTER_COMMAND);
	command |= PCI_COMMAND_BUSMASTER;
	pci_write_word(dev.bus, dev.device, dev.function, PCI_REGISTER_COMMAND, command);

	ac97_reset(pci_dev);

	// Set master volume
	outw(pci_dev.bars[0].io_base + NAM_MASTER_VOL, 0x0000);
	// Set PCM output volume
	outw(pci_dev.bars[0].io_base + NAM_PCM_VOL, 0x0000);

	/* Read capabilities */
	uint8_t channel_capabilities = (inl(pci_dev.bars[1].io_base + GLOB_STA) >> 20) & 0x2;
	uint8_t sample_capabilities = (inl(pci_dev.bars[1].io_base + GLOB_STA) >> 22) & 0x2;
	uint8_t channel_count = 2;

	if (channel_capabilities == 1) {
		channel_count = 4;
	} else if (channel_capabilities == 2) {
		channel_count = 6;
	} else if (channel_capabilities == 3) {
		channel_count = 10;
	}

	kdebug("[ac97] Channels: %d Samples: %s\r\n", channel_count, (sample_capabilities == 1) ? "16bit, 20bit" : "16bit");

	buf_descriptors_rp = 0;
	buf_descriptors_wp = 0;
}
