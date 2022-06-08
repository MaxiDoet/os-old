#ifndef _ac97_h
#define _ac97_h

#include "../include/drivers/pci.h"

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

void ac97_init(pci_dev_descriptor pci_dev);

#endif
