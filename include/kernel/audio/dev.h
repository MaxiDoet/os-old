#ifndef _audio_dev_h
#define _audio_dev_h

enum audio_dev_type {
    AUDIO_DEV_TYPE_OUTPUT = 0,
    AUDIO_DEV_TYPE_INPUT = 1
};

typedef struct audio_dev_t {
    char *name;
    enum audio_dev_type type;

    /* Driver */
    void (*driver_play) (uint8_t *data, uint32_t size);
} audio_dev_t;

void audio_init();
void audio_dev_add(audio_dev_t dev);
void audio_dev_select(uint16_t id);
void audio_dev_play(uint8_t *data, uint32_t size);

#endif