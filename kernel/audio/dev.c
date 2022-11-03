#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/audio/dev.h"
#include "../libc/include/string.h"

audio_dev_t audio_devs[10];
uint8_t audio_devs_index = 0;

void audio_dev_add(audio_dev_t dev)
{
    audio_devs[audio_devs_index] = dev;
}