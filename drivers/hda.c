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

void hda_rirb_read(hda_dev_t *dev, uint64_t *response);

void hda_reset(hda_dev_t *dev)
{
	// Stop all DMA transfers and wait
	hda_wl(dev, HDA_CORBCTL, 0);
	hda_wl(dev, HDA_RIRBCTL, 0);

	while ((hda_rw(dev, HDA_CORBCTL) & HDA_CORBRUN) || (hda_rw(dev, HDA_RIRBCTL) & HDA_RIRBRUN)) {
		kdebug("[hda] stopping dma transfers\r\n");
	}

	// Wait until hardware is in reset state
	hda_wl(dev, HDA_GCTL, 0);
	while (hda_rl(dev, HDA_GCTL) & HDA_CRST) {
        };

	// Wait until hardware is out of reset state
	hda_wl(dev, HDA_GCTL, HDA_CRST);
	while (!(hda_rl(dev, HDA_GCTL) & HDA_CRST)) {
	};
}

void hda_irq_handler()
{
	uint32_t intsts = hda_rl(&dev, HDA_INTSTS);
	uint16_t statests = hda_rw(&dev, HDA_STATESTS);

	kdebug("[hda] irq INTSTS: %x STATESTS: %x\r\n", intsts, statests);

	// Clear interrupts
	hda_wl(&dev, HDA_INTSTS, intsts);
	hda_ww(&dev, HDA_STATESTS, statests);
}

void hda_init_corb(hda_dev_t *dev)
{
	// Stop CORB
	hda_cb(dev, HDA_CORBCTL, HDA_CORBRUN);

	uint8_t corbsize = hda_rb(dev, HDA_CORBSIZE);

	kdebug("[hda] CORBSIZE: ");
	switch(corbsize >> 4) {
		case 0:
			kdebug("2 entries (8 Byte)");
			corbsize |= 0x0;
			dev->corb_size = 2;
			break;
		case 2:
			kdebug("16 entries (64 Byte)");
			corbsize |= 0x1;
			dev->corb_size = 16;
			break;
		case 4:
			kdebug("256 entries (1 Kbyte)");
			corbsize |= 0x2;
			dev->corb_size = 256;
			break;
	}
	kdebug("\r\n");

	// Set CORBSIZE
        hda_wb(dev, HDA_CORBSIZE, corbsize);

	// Allocate CORB
	//dev->corb = (uint32_t *) malloc(4 * dev->corb_size);
	dev->corb = (uint32_t *) 0xA1FD80;

	// Set CORBBASE registers
	//hda_wl(dev, HDA_CORBLBASE, (uint32_t) &dev->corb[0] & 0xFFFFFFFF);
	hda_wl(dev, HDA_CORBLBASE, (uint32_t) 0xA1FD80 & 0xFFFFFFFF);
	hda_wl(dev, HDA_CORBUBASE, (uint32_t) 0x00000000);

	// Wait for hardware to reset CORBRP
	hda_sw(dev, HDA_CORBRP, HDA_CORBRPRST);
	while (!(hda_rw(dev, HDA_CORBRP) & HDA_CORBRPRST)) {
	}
	hda_cw(dev, HDA_CORBRP, HDA_CORBRPRST);
	while (hda_rw(dev, HDA_CORBRP) & HDA_CORBRPRST) {
        }

	// Reset CORBWP
	hda_ww(dev, HDA_CORBWP, 0x0000);

	// Start CORB
	hda_wb(dev, HDA_CORBCTL, HDA_CORBRUN);
}

void hda_init_rirb(hda_dev_t *dev)
{
	// Stop RIRB
        hda_cb(dev, HDA_RIRBCTL, HDA_RIRBRUN);

        uint8_t rirbsize = hda_rb(dev, HDA_RIRBSIZE);

        kdebug("[hda] RIRBSIZE: ");
        switch(rirbsize >> 4) {
                case 0:
                        kdebug("2 entries (16 Byte)");
			rirbsize |= 0x0;
			dev->rirb_size = 2;
			break;
                case 2:
                        kdebug("16 entries (128 Byte)");
			rirbsize |= 0x1;
			dev->rirb_size = 16;
                        break;
                case 4:
                        kdebug("256 entries (2 Kbyte)");
			rirbsize |= 0x2;
			dev->rirb_size = 256;
                        break;
        }
        kdebug("\r\n");

	// Set RIRBSIZE
        hda_wb(dev, HDA_RIRBSIZE, rirbsize);

	// Allocate RIRB
        //dev->rirb = (uint64_t *) malloc(8 * dev->rirb_size);
	dev->rirb = (uint64_t *) 0xA20000;

	// Reset RIRBWP
        hda_sw(dev, HDA_RIRBWP, HDA_RIRBWPRST);

        // Set RIRBBASE registers
       	hda_wl(dev, HDA_RIRBLBASE, (uint32_t) 0xA20000 & 0xFFFFFFFF);
        hda_wl(dev, HDA_RIRBUBASE, (uint32_t) 0x00000000);

	// Set RINTCNT register
	hda_ww(dev, HDA_RINTCNT, 0x1);

	// Enable interrupts and DMA
	hda_sb(dev, HDA_RIRBCTL, HDA_RIRBRUN);
}

void hda_corb_write(hda_dev_t *dev, uint32_t data)
{
	uint16_t wp = hda_rw(dev, HDA_CORBWP) & 0xFF;
	uint16_t next_p = (wp + 1) % dev->corb_size;

	// Wait until dma is ready
	while (next_p == (hda_rw(dev, HDA_CORBRP) & 0xFF)) {
		kdebug("[hda] waiting for dma to be ready\r\n");
	}

	dev->corb[next_p] = data;

	// Update CORBWP
	hda_ww(dev, HDA_CORBWP, next_p);
}

void hda_rirb_read(hda_dev_t *dev, uint64_t *response)
{
	uint16_t rp = dev->rirb_rp;
	uint16_t next_p = (rp + 1) % dev->rirb_size;

	// Wait for new entry
	while (rp == hda_rw(dev, HDA_RIRBWP) & 0xFF) {
		kdebug("[hda] waiting for new entry\r\n");
	}

	rp = next_p;
	dev->rirb_rp = rp;

	// Clear interrupt flags
	hda_sb(dev, HDA_RIRBSTS, HDA_RINTFL | HDA_RIRBOIS);

	*response = dev->rirb[rp];
}

hda_dev_t hda_init(pci_dev_descriptor pci_dev)
{
	dev.mem_base = pci_dev.bars[0].mem_base;

	hda_reset(&dev);

	dev.gcap = hda_rw(&dev, HDA_GCAP);
	uint8_t major = hda_rb(&dev, HDA_VMAJ);
	uint8_t minor = hda_rb(&dev, HDA_VMIN);
	uint8_t oss = (dev.gcap >> 12) & 0xF;
	uint8_t iss = (dev.gcap >> 8) & 0xF;

	kdebug("[hda] Version: %d.%d OSS: %d ISS: %d\r\n", major, minor, oss, iss);

	// Enable pci bus mastering
	pci_enable_bus_mastering(pci_dev);

	// Scan for codecs
	uint16_t statests = hda_rw(&dev, HDA_STATESTS);
	for (int i=0; i < 15; i++) {
		if (statests & (1 << i)) {

			kdebug("[hda] found codec %x\r\n", i);
		}
	}

	// Enable wake events
        hda_ww(&dev, HDA_WAKEEN, 0xFFFF);

	// Enable interrupts
	irq_install_handler(pci_dev.irq, hda_irq_handler);
	hda_wl(&dev, HDA_INTCTL, HDA_GIE | HDA_CIE | 0xFF);

	hda_init_corb(&dev);
	hda_init_rirb(&dev);

	hda_corb_write(&dev, 0xF0002);

	uint64_t response;
	hda_rirb_read(&dev, &response);
	kdebug("[hda] RIRB: %x\r\n", response);
}
