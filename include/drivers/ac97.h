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
#define NABM_GLOBAL_CTL 0x2C
#define NABM_GLOBAL_STS 0x30
#define NABM_BUFFER_CTN 0x0B

/*
#define NABM_BUFFER_DSC_ADDR 0x00
#define NABM_CUR_ENTRY_VAL 0x04
#define NABM_LAST_VALID_ENTRY 0x05
#define NABM_TRANSFER_STS 0x06
#define NABM_CUR_IDX_PROC_SAMPLES 0x08
#define NABM_PRCSD_ENTRY 0x0A
#define NABM_BUFFER_CNT 0x0B
*/

/* NABM_GLOBAL_CTL */
#define CTL_GIE (1 << 0)
#define CTL_COLD_RESET (1 << 1)
#define CTL_WARM_RESET (1 << 2)
#define CTK_SHUDOWN (1 << 3)

void ac97_init(pci_dev_descriptor pci_dev);

#endif
