#ifndef _fs_fs_h
#define _fs_fs_h

#include "../include/kernel/dd/dev.h"

enum fs_type {
    FS_TYPE_EXT2 = 0,
    FS_TYPE_MEMFS = 1
};

typedef struct fs_t {
    enum fs_type type;
    dd_dev_t *dev;

    uint8_t *data; // This points to fs data
} fs_t;

typedef struct fs_file_t {
    fs_t *fs;
    char *path;
} fs_file_t;

fs_file_t fs_open(fs_t *fs, char *path);
uint8_t fs_read(fs_file_t file, uint8_t *buf);
uint32_t fs_size(fs_file_t file);

#endif
