#ifndef _hda_h
#define _hda_h

#include "../include/drivers/pci.h"

typedef struct hda_corb_entry_t {
        uint8_t payload;
	uint16_t verb_id : 12;
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

/* Verbs */
#define HDA_VERB_GET_PARAMETER 0xF00

/* GET_PARAMETER parameters */
#define HDA_VERB_PARAMETER_VENDOR_ID 0x00
#define HDA_VERB_PARAMETER_REV_ID 0x02
#define HDA_VERB_PARAMETER_NODE_COUNT 0x04
#define HDA_VERB_PARAMETER_FUNCTION_GROUP_TYPE 0x05
#define HDA_VERB_PARAMETER_AUDIO_FUNCTION_GROUP_CAP 0x08
#define HDA_VERB_PARAMETER_AUDIO_WIDGET_CAP 0x09
#define HDA_VERB_PARAMETER_SAMPLE_SIZE 0x0A

hda_dev_t hda_init(pci_dev_descriptor pci_dev);

/* MMIO */

static inline void hda_wb(hda_dev_t *dev, uint8_t reg, uint8_t val)
{
        volatile uint8_t* out = (uint8_t*) ((uint8_t*) dev->mem_base + reg);
        *out = val;
}

static inline void hda_ww(hda_dev_t *dev, uint8_t reg, uint16_t val)
{
        volatile uint16_t* out = (uint16_t*) ((uint8_t*) dev->mem_base + reg);
        *out = val;
}

static inline void hda_wl(hda_dev_t *dev, uint8_t reg, uint32_t val)
{
        volatile uint32_t* out = (uint32_t*) ((uint8_t*) dev->mem_base + reg);
        *out = val;
}

static inline uint8_t hda_rb(hda_dev_t *dev, uint8_t reg)
{
        volatile uint8_t* in = (uint8_t*) ((uint8_t*) dev->mem_base + reg);
        return *in;
}

static inline uint16_t hda_rw(hda_dev_t *dev, uint8_t reg)
{
        volatile uint16_t* in = (uint16_t*) ((uint8_t*) dev->mem_base + reg);
        return *in;
}

static inline uint32_t hda_rl(hda_dev_t *dev, uint8_t reg)
{
        volatile uint32_t* in = (uint32_t*) ((uint8_t*) dev->mem_base + reg);
        return *in;
}

static inline void hda_sb(hda_dev_t *dev, uint8_t reg, uint8_t mask)
{
        volatile uint8_t* out = (uint8_t*) ((uint8_t*) dev->mem_base + reg);
        *out |= mask;
}

static inline void hda_sw(hda_dev_t *dev, uint8_t reg, uint16_t mask)
{
        volatile uint16_t* out = (uint16_t*) ((uint8_t*) dev->mem_base + reg);
        *out |= mask;
}

static inline void hda_sl(hda_dev_t *dev, uint8_t reg, uint32_t mask)
{
        volatile uint32_t* out = (uint32_t*) ((uint8_t*) dev->mem_base + reg);
        *out |= mask;
}

static inline void hda_cb(hda_dev_t *dev, uint8_t reg, uint8_t mask)
{
        volatile uint8_t* out = (uint8_t*) ((uint8_t*) dev->mem_base + reg);
        *out &= ~mask;
}

static inline void hda_cw(hda_dev_t *dev, uint8_t reg, uint16_t mask)
{
        volatile uint16_t* out = (uint16_t*) ((uint8_t*) dev->mem_base + reg);
        *out &= ~mask;
}

static inline void hda_cl(hda_dev_t *dev, uint8_t reg, uint32_t mask)
{
        volatile uint32_t* out = (uint32_t*) ((uint8_t*) dev->mem_base + reg);
        *out &= ~mask;
}

#endif
