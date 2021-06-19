#ifndef _rtl8139_h
#define _rtl8139_h

#include "../include/drivers/pci.h"

// Registers
#define REG_CR 0x37
#define REG_CAPR 0x38
#define REG_IMR 0x3C
#define REG_ISR 0x3E
#define REG_TCR 0x40
#define REG_RCR 0x44
#define REG_RBSTART 0x30

// Buffers
#define TX_BUFFER_SIZE 1792
#define RX_BUFFER_SIZE 9744

// Interrupt mask register masks
#define IMR_SERR (1 << 15)
#define IMR_TIMEOUT (1 << 14)
#define IMR_LENCHG (1 << 13)
#define IMR_FOVW (1 << 6)
#define IMR_PUN (1 << 5)
#define IMR_RXOVW (1 << 4)
#define IMR_TER (1 << 3)
#define IMR_TOK (1 << 2)
#define IMR_RER (1 << 1)
#define IMR_ROK (1 << 0)

// Transmit start address registers
#define REG_TSAD0 0x20
#define REG_TSAD1 0x24
#define REG_TSAD2 0x28
#define REG_TSAD3 0x2C

// Transmit status registers
#define REG_TSD0 0x10
#define REG_TSD1 0x14
#define REG_TSD2 0x18
#define REG_TSD3 0x1C

// Config registers
#define REG_CONFIG0 0x51
#define REG_CONFIG1 0x52
#define REG_CONFIG3 0x59
#define REG_CONFIG4 0x5A

// Command register
#define CR_RST (1 << 4)
#define CR_RE (1 << 3)
#define CR_TE (1 << 2)
#define CR_BUFE (1 << 0)

// RX config
#define RCR_ACCEPT_PHYSICAL_ADDRESS_PACKETS (1 << 0)
#define RCR_ACCEPT_PHYSICAL_MATCH_PACKETS (1 << 1)
#define RCR_ACCEPT_MULTICAST_PACKETS (1 << 2)
#define RCR_ACCEPT_BROADCAST_PACKETS (1 << 3)
#define RCR_ACCEPT_RUNT_PACKETS (1 << 4)
#define RCR_ACCEPT_ERROR_PACKETS (1 << 5)
#define RCR_WRAP (1 << 7)

void rtl8139_init(pci_dev_descriptor dev);

#endif
