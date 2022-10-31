#ifndef _desktop_h
#define _desktop_h

#include "../include/kernel/multiboot.h"
#include "../include/kernel/fs/vfs.h"

void desktop_init(multiboot_info_t *mbi, vfs_fs_t *root_fs);

#endif
