#ifndef _hda_h
#define _hda_h

#include "../include/drivers/pci.h"

typedef struct hda_corb_entry_t {
        uint32_t payload : 20;
        uint8_t node_id;
        uint8_t codec_addr : 4;
} __attribute__((packed)) hda_corb_entry_t;

typedef struct hda_rirb_entry_t {
	uint32_t response;
	uint32_t response_ex;
} __attribute__((packed)) hda_rirb_entry_t;

typedef struct hda_dev_t {
	uint32_t mem_base;
	uint16_t gcap;

	/* Corb */
	//hda_corb_entry_t *corb;
	hda_corb_entry_t corb[256];
	uint16_t corb_wp;
	uint16_t corb_rp;
	int corb_size;

	/* RIRB */
	//hda_rirb_entry_t *rirb;
	hda_rirb_entry_t rirb[256];
	uint16_t rirb_rp;
	int rirb_size;
} hda_dev_t;

#define HDA_GCAP 0x00
#define HDA_VMIN 0x02
#define HDA_VMAJ 0x03
#define HDA_GCTL 0x08
#define HDA_WAKEEN 0x0C
#define HDA_STATESTS 0x0E
#define HDA_INTCTL 0x20
#define HDA_INTSTS 0x24
#define HDA_CORBLBASE 0x40
#define HDA_CORBUBASE 0x44
#define HDA_CORBWP 0x48
#define HDA_CORBRP 0x4A
#define HDA_CORBCTL 0x4C
#define HDA_CORBSTS 0x4D
#define HDA_CORBSIZE 0x4E
#define HDA_RIRBLBASE 0x50
#define HDA_RIRBUBASE 0x54
#define HDA_RIRBWP 0x58
#define HDA_RINTCNT 0x5A
#define HDA_RIRBCTL 0x5C
#define HDA_RIRBSTS 0x5D
#define HDA_RIRBSIZE 0x5E

/* GCTL */
#define HDA_CRST (1 << 0)
#define HDA_FCNTRL (1 << 1)
#define HDA_UNSOL (1 << 8)

/* INTCTL */
#define HDA_CIE (1 << 30)
#define HDA_GIE (1 << 31)

/* CORBCTL */
#define HDA_CORBRUN (1 << 1)

/* CORBRP */
#define HDA_CORBRPRST (1 << 15)

/* RIRBWP */
#define HDA_RIRBWPRST (1 << 15)

/* RIRBCTL */
#define HDA_RINTCTL (1 << 0)
#define HDA_RIRBRUN (1 << 1)

/* RIRBSTS */
#define HDA_RINTFL (1 << 0)
#define HDA_RIRBOIS (1 << 2)

hda_dev_t hda_init(pci_dev_descriptor pci_dev);

#endif
