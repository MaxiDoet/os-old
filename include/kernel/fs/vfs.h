#ifndef _fs_vfs_h
#define _fs_vfs_h

#include "../include/drivers/ata.h"

enum vfs_fs_type {
    VFS_FS_TYPE_EXT2 = 0,
    VFS_FS_TYPE_MEMFS = 1
};

typedef struct vfs_fs_t {
    enum vfs_fs_type type;
    ata_dev_t *dev;

    uint8_t *data; // This points to fs data
} vfs_fs_t;

uint8_t vfs_read(vfs_fs_t *fs, char* path, uint8_t *buf);

#endif
