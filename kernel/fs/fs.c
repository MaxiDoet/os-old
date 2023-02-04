#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/kernel.h"
#include "../include/kernel/mem/heap.h"
#include "../include/kernel/fs/fs.h"
#include "../include/kernel/fs/gpt.h"
#include "../include/kernel/fs/ext2.h"
#include "../include/drivers/ata.h"

fs_file_t fs_open(fs_t *fs, char *path)
{
	fs_file_t file;

	file.fs = fs;
	file.path = path;

	return file;
}

uint8_t fs_read(fs_file_t file, uint8_t *buf)
{
	uint8_t result = 0;

	switch(file.fs->type) {
		case FS_TYPE_EXT2:
			result = ext2_read(file.fs->dev, (ext2_fs_t *) file.fs->data, file.path, buf);
			break;
		
		case FS_TYPE_MEMFS:
			result = 0;
			break;
	}

	return result;
}

uint32_t fs_size(fs_file_t file)
{
	uint32_t result = 0;

	switch(file.fs->type) {
		case FS_TYPE_EXT2:
			result = ext2_size(file.fs->dev, (ext2_fs_t *) file.fs->data, file.path);
			break;
		
		case FS_TYPE_MEMFS:
			result = 0;
			break;
	}

	return result;
}
