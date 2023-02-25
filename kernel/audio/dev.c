#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/io/pci.h"
#include "../include/kernel/audio/dev.h"
#include "../include/drivers/hda.h"
#include "../include/drivers/ac97.h"
#include "../libc/include/string.h"

audio_dev_t audio_dev_list[1024];
uint16_t audio_dev_list_index = 0;
uint16_t audio_dev_current = 0;

void audio_init()
{
    pci_dev_t *list = (pci_dev_t *) malloc(sizeof(pci_dev_t) * 1024);
	uint16_t list_length = 0;

    // Audio / AC97
	list_length = pci_get_device_by_id(list, 0x8086, 0x2415);
	for (uint16_t i=0; i < list_length; i++) {
		ac97_init(list[i]);
	}

	// Audio / Intel HDA
	list_length = pci_get_device_by_id(list, 0x8086, 0x2668);
	for (uint16_t i=0; i < list_length; i++) {
		hda_init(list[i]);
	}

	list_length = pci_get_device_by_id(list, 0x8086, 0x293E);
	for (uint16_t i=0; i < list_length; i++) {
		hda_init(list[i]);
	}

	free(list);

    audio_dev_select(0);
}

void audio_dev_add(audio_dev_t dev)
{
    audio_dev_list[audio_dev_list_index++] = dev;

    kdebug("[audio] Registered new device | %s\r\n", dev.name);
}

void audio_dev_select(uint16_t id)
{
    audio_dev_current = id;
    audio_dev_t *dev = &audio_dev_list[id];

    kdebug("[audio] Select %s (%d)\r\n", dev->name, id);
}

audio_return_t audio_dev_play(uint8_t *data, uint32_t size)
{
	if (audio_dev_list_index == 0) {
		return AUDIO_RETURN_DEVICE_NOT_FOUND;
	}

	audio_dev_t *dev = &audio_dev_list[audio_dev_current];
	(*dev->driver_play)(data, size);
}