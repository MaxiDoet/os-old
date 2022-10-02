#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/hda.h"
#include "../include/kernel/kernel.h"
#include "../include/kernel/irq.h"
#include "../include/drivers/pci.h"
#include "../include/kernel/io.h"
#include "../include/kernel/mem/heap.h"
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
	//dev->corb = (uint32_t *) malloc(sizeof(hda_corb_entry_t) * dev->corb_size);
	dev->corb = (uint32_t *) 0xA1FD80;

	// Set CORBBASE registers
	//hda_wl(dev, HDA_CORBLBASE, (uint32_t) dev->corb & 0xFFFFFFFF);
	//kdebug("%x\r\n", (uint32_t) dev->corb & 0xFFFFFFFF);
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
    //hda_wl(dev, HDA_RIRBLBASE, (uint32_t) dev->rirb & 0xFFFFFFFF);
	//kdebug("%x\r\n", (uint32_t) dev->rirb & 0xFFFFFFFF);
	hda_wl(dev, HDA_RIRBLBASE, (uint32_t) 0xA20000 & 0xFFFFFFFF);
    hda_wl(dev, HDA_RIRBUBASE, (uint32_t) 0x00000000);

	// Set RINTCNT register
	hda_ww(dev, HDA_RINTCNT, 0x42);

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

uint32_t hda_codec_query(hda_dev_t *dev, int codec, int nid, uint32_t payload)
{
	uint32_t data = ((codec & 0xF) << 28) | ((nid & 0xFF) << 20) | (payload & 0xFFFFF);
	uint64_t response;

	hda_corb_write(dev, data);
	hda_rirb_read(dev, &response);

	return response;
}

void hda_output_init(hda_dev_t *dev, int codec, int nid)
{
	dev->output.codec = codec;
	dev->output.nid = nid;
	dev->output_channels = 1;

	// Select stream
	hda_codec_query(dev, codec, nid, HDA_VERB_SET_STREAM_CHANNEL | 0x10);

	// Set stream format
	uint16_t format = (1 << 4) | (dev->output_channels - 1);
	hda_codec_query(dev, codec, nid, HDA_VERB_SET_FORMAT | format);
	hda_ww(dev, HDA_SD0FMT, format);
}

void hda_pin_config_print(uint32_t config)
{
	switch((config >> 28) & 0x3) {
		case HDA_WIDGET_LOCATION_EXTERNAL:
			kdebug("External");
			break;
		case HDA_WIDGET_LOCATION_INTERNAL:
			kdebug("Internal");
			break;
		case HDA_WIDGET_LOCATION_SEPARATE:
			kdebug("Separate");
			break;
		case HDA_WIDGET_LOCATION_OTHER:
			kdebug("Other Location");
			break;
		default:
			break;
	}
	kdebug(" ");

	switch((config >> 24) & 0xF) {
		case HDA_WIDGET_LOCATION_NA:
			kdebug("N/A");
			break;
		case HDA_WIDGET_LOCATION_REAR:
			kdebug("Rear");
			break;
		case HDA_WIDGET_LOCATION_FRONT:
			kdebug("Front");
			break;
		case HDA_WIDGET_LOCATION_LEFT:
			kdebug("Left");
			break;
		case HDA_WIDGET_LOCATION_RIGHT:
			kdebug("Right");
			break;
		case HDA_WIDGET_LOCATION_TOP:
			kdebug("Top");
			break;
		case HDA_WIDGET_LOCATION_BOTTOM:
			kdebug("Bottom");
			break;
		case HDA_WIDGET_LOCATION_SPECIAL:
			kdebug("Special");
			break;
		default:
			break;
	}
	kdebug(" ");

	switch((config >> 20) & 0xF) {
		case HDA_WIDGET_DEFAULT_LINEOUT:
			kdebug("Line Out");
			break;
		case HDA_WIDGET_DEFAULT_SPEAKER:
			kdebug("Speaker");
			break;
		case HDA_WIDGET_DEFAULT_HPOUT:
			kdebug("Headphones");
			break;
		case HDA_WIDGET_DEFAULT_LINEIN:
			kdebug("Line In");
			break;
		case HDA_WIDGET_DEFAULT_MICIN:
			kdebug("Microphone");
			break;
		default:
			break;
	}
	kdebug(" ");
}

void hda_widget_init(hda_dev_t *dev, int codec, int nid)
{
	uint32_t widget_cap = hda_codec_query(dev, codec, nid, HDA_VERB_GET_PARAMETER | HDA_VERB_PARAMETER_AUDIO_WIDGET_CAP);

	uint8_t type = (widget_cap >> 20) & 0xF;

	kdebug("[hda] codec: %d nid: %d | ", codec, nid);

	switch(type) {
		case HDA_WIDGET_TYPE_OUTPUT:
			kdebug("Audio Output");

			if (!dev->output.codec || !dev->output.nid) {
				kdebug(" (Default)");
				hda_output_init(dev, codec, nid);
			}
			break;
		case HDA_WIDGET_TYPE_INPUT:
			kdebug("Audio Input");
			break;
		case HDA_WIDGET_TYPE_MIXER:
			kdebug("Audio Mixer");
			break;
		case HDA_WIDGET_TYPE_PIN:
			kdebug("Pin Complex | ");

			// Read config
			uint32_t config = hda_codec_query(dev, codec, nid, HDA_VERB_GET_CONFIG);
			hda_pin_config_print(config);
			break;
		default:
			kdebug("Unknown");
	}
	kdebug("\r\n");
}

void hda_codec_scan(hda_dev_t *dev, int codec)
{
	uint32_t temp = hda_codec_query(dev, codec, 0, HDA_VERB_GET_PARAMETER | HDA_VERB_PARAMETER_NODE_COUNT);
	int fg_total = temp & 0xFF;
	int fg_start = (temp >> 16) & 0xFF;

	for (int i=0; i < fg_total; i++) {
		temp = hda_codec_query(dev, codec, fg_start + i, HDA_VERB_GET_PARAMETER | HDA_VERB_PARAMETER_NODE_COUNT);

		int widgets_total = temp & 0xFF;
		int widgets_start = (temp >> 16) & 0xFF;

		temp = hda_codec_query(dev, codec, fg_start + i, HDA_VERB_GET_PARAMETER | HDA_VERB_PARAMETER_FUNCTION_GROUP_TYPE);
                temp &= 0x7F;

		if (temp == HDA_FG_TYPE_AUDIO) {
			// Set powerstate to D0
			hda_codec_query(dev, codec, fg_start + i, HDA_VERB_SET_POWER_STATE | 0x0);
		}

		for (int j=0; j < widgets_total; j++) {
			hda_widget_init(dev, codec, widgets_start + j);
		}
	}
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

	uint16_t statests = hda_rw(&dev, HDA_STATESTS);

	/* Enable bus mastering */
	uint16_t command = pci_read_word(pci_dev.bus, pci_dev.device, pci_dev.function, PCI_REGISTER_COMMAND);
	command |= PCI_COMMAND_BUSMASTER;
	pci_write_word(pci_dev.bus, pci_dev.device, pci_dev.function, PCI_REGISTER_COMMAND, command);

	// Enable wake events
    hda_ww(&dev, HDA_WAKEEN, 0xFFFF);

	// Enable interrupts
	irq_install_handler(pci_dev.irq, hda_irq_handler);
	hda_wl(&dev, HDA_INTCTL, HDA_GIE | HDA_CIE | 0xFF);

	hda_init_corb(&dev);
	hda_init_rirb(&dev);

	// Scan for codecs
	for (int i=0; i < 15; i++) {
		if (statests & (1 << i)) {
			hda_codec_scan(&dev, i);
		}
	}
}
