#ifndef _hda_h
#define _hda_h

#include "../include/kernel/io/pci.h"

typedef struct hda_widget_t {
	int codec;
	int nid;
} hda_widget_t;

typedef struct hda_dev_t {
	uint32_t mem_base;
	uint16_t gcap;

	/* Corb */
	//uint32_t corb[256];
	uint32_t *corb;
	uint16_t corb_wp;
	uint16_t corb_rp;
	int corb_size;

	/* RIRB */
	//uint64_t rirb[256];
	uint64_t *rirb;
	uint16_t rirb_rp;
	int rirb_size;

	// Output
	hda_widget_t output;
	int output_channels;
} hda_dev_t;

hda_dev_t hda_init(pci_dev_t pci_dev);

#endif
