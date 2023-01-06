#ifndef _lib_wav_h
#define _lib_wav_h

#include <stdint.h>

typedef struct wave_chunk_t {
    char chunk_id[4];
    uint32_t chunk_size;
} __attribute__((packed)) wave_chunk_t;

typedef struct wave_riff_chunk_t {
    char riff_type[4];    
} __attribute__((packed)) wave_riff_chunk_t;

typedef struct wave_format_chunk_t {
    uint16_t format_tag;
    uint16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    uint16_t block_align;
    uint16_t bits_per_sample;
} __attribute__((packed)) wave_format_chunk_t;

#define WAVE_RIFF_SIGNATURE     0x52494646
#define WAVE_FORMAT_SIGNATURE   0x666D7420
#define WAVE_DATA_SIGNATURE     0x64617461

#endif