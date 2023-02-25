#ifndef _pit_h
#define _pit_h

#define PIT_CHANNEL_0_DATA 0x40
#define PIT_CHANNEL_1_DATA 0x41
#define PIT_CHANNEL_2_DATA 0x42
#define PIT_MODE_PORT 0x43

void pit_init();
void pit_sleep(uint32_t ticks);

#endif
