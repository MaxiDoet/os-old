#ifndef _audio_dev_h
#define _audio_dev_h

enum audio_dev_type {
    AUDIO_DEV_TYPE_OUTPUT = 0,
    AUDIO_DEV_TYPE_INPUT = 1
};

typedef struct audio_dev_t {
    char *name;
    enum audio_dev_type type;


} audio_dev_t;

void audio_dev_add();

#endif