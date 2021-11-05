#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/hda.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/drivers/pci.h"
#include "../include/kernel/io.h"
#include "../libc/include/mm.h"
#include "../libc/include/string.h"

static hda_dev_t dev;

void hda_rirb_read(hda_dev_t *dev, hda_rirb_entry_t *response);

void hda_reset(hda_dev_t dev)
{
	// Wait until hardware is in reset state
	*(uint32_t *) (dev.mem_base + HDA_GCTL) = 0;
	while (*(uint32_t *) (dev.mem_base + HDA_GCTL) & HDA_CRST) {
        };

	// Wait until hardware is out of reset state
	*(uint32_t *) (dev.mem_base + HDA_GCTL) |= HDA_CRST;
	while (*(uint32_t *) (dev.mem_base + HDA_GCTL) & ~HDA_CRST) {
	};
}

void hda_irq_handler()
{
	uint32_t intsts = *(uint32_t *) (dev.mem_base + HDA_INTSTS);
	uint16_t statests = *(uint16_t *) (dev.mem_base + HDA_STATESTS);

	kdebug("[hda] irq INTSTS: %x STATESTS: %x\r\n", intsts, statests);

	// Clear interrupt
	*(uint32_t *) (dev.mem_base + HDA_INTSTS) = intsts;
	*(uint16_t *) (dev.mem_base + HDA_STATESTS) = statests;
}

void hda_init_corb(hda_dev_t *dev)
{
	// Stop CORB
	*(uint8_t *) (dev->mem_base + HDA_CORBCTL) &= ~HDA_CORBRUN;

	uint8_t corbsize = *(uint8_t *) (dev->mem_base + HDA_CORBSIZE);

	kdebug("[hda] CORBSZCAP: ");
	switch(corbsize >> 4) {
		case 0:
			kdebug("2 entries (8 Byte)");
			corbsize |= 0x00;
			break;
		case 2:
			kdebug("16 entries (64 Byte)");
			corbsize |= 0x01;
			break;
		case 4:
			kdebug("256 entries (1 Kbyte)");
			corbsize |= 0x02;
			break;
	}

	kdebug("\r\n[hda] CORBSIZE: ");
	switch(corbsize & 0x3) {
		case 0:
			kdebug("2 entries (8 Byte)");
			dev->corb_size = 2;
			//dev->corb = (hda_corb_entry_t *) malloc(sizeof(hda_corb_entry_t) * 2);
			break;
		case 1:
			kdebug("16 entries (64 Byte)");
			dev->corb_size = 16;
			//dev->corb = (hda_corb_entry_t *) malloc(sizeof(hda_corb_entry_t) * 16);
			break;
		case 2:
			kdebug("256 entries (1 Kbyte)");
			dev->corb_size = 256;
			//dev->corb = (hda_corb_entry_t *) malloc(sizeof(hda_corb_entry_t) * 256);
			break;
	}
	kdebug("\r\n");

	// Set CORBSIZE
        *(uint8_t *) (dev->mem_base + HDA_CORBSIZE) = corbsize;

	// Set CORBBASE registers
	*(uint32_t *) (dev->mem_base + HDA_CORBLBASE) = (uint32_t) &dev->corb[0] & 0xFFFFFFFF;
	*(uint32_t *) (dev->mem_base + HDA_CORBUBASE) = (uint32_t) 0x00000000;

	// Wait for hardware to reset CORBRP
	*(uint16_t *) (dev->mem_base + HDA_CORBRP) |= HDA_CORBRPRST;
	while (!(*(uint16_t *) (dev->mem_base + HDA_CORBRP) & HDA_CORBRPRST)) {
	}
	*(uint16_t *) (dev->mem_base + HDA_CORBRP) &= ~HDA_CORBRPRST;
	while (*(uint16_t *) (dev->mem_base + HDA_CORBRP) & HDA_CORBRPRST) {
        }

	// Reset CORBWP
	*(uint16_t *) (dev->mem_base + HDA_CORBWP) = 0x0000;

	// Start CORB
	*(uint8_t *) (dev->mem_base + HDA_CORBCTL) = HDA_CORBRUN;
}

void hda_init_rirb(hda_dev_t *dev)
{
	// Stop RIRB
        *(uint8_t *) (dev->mem_base + HDA_RIRBCTL) &= ~HDA_RIRBRUN;

        uint8_t rirbsize = *(uint8_t *) (dev->mem_base + HDA_RIRBSIZE);

        kdebug("[hda] RIRBSZCAP: ");
        switch(rirbsize >> 4) {
                case 0:
                        kdebug("2 entries (16 Byte)");
			rirbsize |= 0x00;
			break;
                case 2:
                        kdebug("16 entries (128 Byte)");
			rirbsize |= 0x01;
                        break;
                case 4:
                        kdebug("256 entries (2 Kbyte)");
			rirbsize |= 0x02;
                        break;
        }

        kdebug("\r\n[hda] RIRBSIZE: ");
        switch(rirbsize & 0x3) {
                case 0:
                        kdebug("2 entries (16 Byte)");
                        dev->rirb_size = 2;
                        //dev->rirb = (hda_rirb_entry_t *) malloc(sizeof(hda_rirb_entry_t) * 2);
                        break;
                case 1:
                        kdebug("16 entries (128 Byte)");
                        dev->rirb_size = 16;
                        //dev->rirb = (hda_rirb_entry_t *) malloc(sizeof(hda_rirb_entry_t) * 16);
                        break;
                case 2:
                        kdebug("256 entries (2 Kbyte)");
                        dev->rirb_size = 256;
                        //dev->rirb = (hda_rirb_entry_t *) malloc(sizeof(hda_rirb_entry_t) * 256);
                        break;
        }
        kdebug("\r\n");

	// Set RIRBSIZE
        *(uint8_t *) (dev->mem_base + HDA_RIRBSIZE) = rirbsize;

	// Reset RIRBWP
        *(uint16_t *) (dev->mem_base + HDA_RIRBWP) = HDA_RIRBWPRST;

        // Set RIRBBASE registers
        *(uint32_t *) (dev->mem_base + HDA_RIRBLBASE) = (uint32_t) &dev->rirb[0] & 0xFFFFFFFF;
        *(uint32_t *) (dev->mem_base + HDA_RIRBUBASE) = (uint32_t) 0x00000000;

	*(uint8_t *) (dev->mem_base + HDA_RINTCNT) = 0x1;

	// Enable interrupts and DMA
	*(uint8_t *) (dev->mem_base + HDA_RIRBCTL) = HDA_RIRBRUN;
}

void hda_corb_write(hda_dev_t *dev, hda_corb_entry_t entry)
{
	uint16_t wp = *(uint16_t *) (dev->mem_base + HDA_CORBWP) & 0xFF;
	uint16_t next_p = (wp + 1) % dev->corb_size;

	// Wait until dma is ready
	while (next_p == *(uint16_t *) (dev->mem_base + HDA_CORBRP) & 0xFF) {
		kdebug("[hda] waiting for dma to be ready\r\n");
	}

	dev->corb[next_p] = entry;
	//memset(dev->corb, 0xff, sizeof(hda_corb_entry_t) * dev->corb_size);

	// Update CORBWP
	*(uint16_t *) (dev->mem_base + HDA_CORBWP) = next_p;
}

void hda_rirb_read(hda_dev_t *dev, hda_rirb_entry_t *response)
{
	uint16_t rp = dev->rirb_rp;
	uint16_t next_p = (rp + 1) % dev->rirb_size;

	// Wait for new entry
	while (rp == *(uint16_t *) (dev->mem_base + HDA_RIRBWP) & 0xFF) {
		kdebug("[hda] waiting for new entry\r\n");
	}

	rp = next_p;
	dev->rirb_rp = rp;

	// Clear interrupt flags
	*(uint8_t *) (dev->mem_base + HDA_RIRBSTS) |= HDA_RINTFL | HDA_RIRBOIS;

	for (int i=0; i < 5; i++) {
		kdebug("rirb[%d]: %x\r\n", i, dev->rirb[rp].response);
	}

	*response = dev->rirb[rp];
}

hda_dev_t hda_init(pci_dev_descriptor pci_dev)
{
	dev.mem_base = pci_dev.bars[0].mem_base;

	hda_reset(dev);

	dev.gcap = *(uint16_t *) (dev.mem_base + HDA_GCAP);
	uint8_t major = *(uint8_t *) (dev.mem_base + HDA_VMAJ);
	uint8_t minor = *(uint8_t *) (dev.mem_base + HDA_VMIN);
	uint8_t oss = (dev.gcap >> 12) & 0xF;
	uint8_t iss = (dev.gcap >> 8) & 0xF;

	kdebug("[hda] Version: %d.%d OSS: %d ISS: %d\r\n", major, minor, oss, iss);

	// Enable pci bus mastering
	pci_enable_bus_mastering(pci_dev);

	// Scan for codecs
	uint16_t statests = *(uint16_t *) (dev.mem_base + HDA_STATESTS);
	for (int i=0; i < 15; i++) {
		if (statests & (1 << i)) {
			kdebug("[hda] found codec %x\r\n", i);
		}
	}

	// Enable interrupts
	irq_install_handler(pci_dev.irq, hda_irq_handler);
	*(uint32_t *) (dev.mem_base + HDA_INTCTL) |= HDA_GIE;
	*(uint32_t *) (dev.mem_base + HDA_INTCTL) |= HDA_CIE;
	*(uint32_t *) (dev.mem_base + HDA_INTCTL) |= 0xFF;

	// Enable wake events
	*(uint16_t *) (dev.mem_base + HDA_WAKEEN) = 0xFFFF;

	hda_init_corb(&dev);
	hda_init_rirb(&dev);

	hda_corb_entry_t test;
	test.payload = 0xf00;
	test.node_id = 0;
	test.codec_addr = 0;

	hda_corb_write(&dev, test);

	hda_rirb_entry_t test2;
        hda_rirb_read(&dev, &test2);
        kdebug("[hda] RIRB: %x\r\n", test2.response);
}
