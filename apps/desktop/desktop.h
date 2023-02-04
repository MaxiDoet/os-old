#ifndef _desktop_h
#define _desktop_h

#include "../include/kernel/multiboot.h"
#include "../include/kernel/fs/fs.h"

void desktop_init(multiboot_info_t *mbi, fs_t *root_fs);

#endif
