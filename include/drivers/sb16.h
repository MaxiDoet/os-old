#ifndef _sb16_h
#define _sb16_h

#define SB16_DSP_MIXER_PORT 0x224
#define SB16_DSP_MIXER_DATA_PORT 0x225
#define SB16_DSP_RESET_PORT 0x226
#define SB16_DSP_READ_PORT 0x22A
#define SB16_DSP_WRITE_PORT 0x22C
#define SB16_DSP_STATUS_PORT 0x22E
#define SB16_DSP_STATUS_16_PORT 0x22F

void sb16_reset();
void sb16_write(uint8_t data);
uint8_t sb16_read();
void sb16_init();
int sb16_probe();

#endif
