#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/fs/vfs.h"
#include "../include/kernel/fs/gpt.h"
#include "../include/kernel/fs/ext2.h"
#include "../include/drivers/ata.h"

uint8_t vfs_read(vfs_fs_t *fs, char* path, uint8_t *buf)
{
	uint8_t result = 0;

	switch(fs->type) {
		case VFS_FS_TYPE_EXT2:
			result = ext2_read_file(fs->dev, (ext2_fs_t *) fs->data, path, buf);
			break;
		
		case VFS_FS_TYPE_MEMFS:
			result = 0;
			break;
	}

	return result;
}
