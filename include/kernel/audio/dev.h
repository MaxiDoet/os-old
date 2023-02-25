#ifndef _audio_dev_h
#define _audio_dev_h

enum audio_dev_type {
    AUDIO_DEV_TYPE_OUTPUT = 0,
    AUDIO_DEV_TYPE_INPUT = 1
};

typedef enum audio_return_t {
    AUDIO_RETURN_SUCCESS = 0,
    AUDIO_RETURN_DEVICE_NOT_FOUND = 1,
    AUDIO_RETURN_HARDWARE_ERROR = 2
} audio_return_t;

typedef struct audio_dev_t {
    char *name;
    enum audio_dev_type type;

    /* Driver */
    audio_return_t (*driver_play) (uint8_t *data, uint32_t size);
} audio_dev_t;

void audio_init();
void audio_dev_add(audio_dev_t dev);
void audio_dev_select(uint16_t id);

audio_return_t audio_dev_play(uint8_t *data, uint32_t size);

#endif