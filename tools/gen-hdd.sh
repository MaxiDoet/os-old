#!/bin/sh

block_size=512
blocks=204800
output_file="hdd.img"
fs_type="ext2"
fs_code="20"

# Create raw image
dd if=/dev/zero of=$output_file bs=$block_size count=$blocks

# Setup mbr and first partition
(
echo g
echo n # Add a new partition
echo   # Partition number (Accept default: 1)
echo   # First sector (Accept default: 1)
echo   # Last sector (Accept default: varies)
echo t # Set partition type
echo $fs_code
echo w # Write changes
) | fdisk $output_file

losetup -P /dev/loop20 $output_file

# Create ext2 filesystem
if [ $fs_type = "ext2" ]; then
	mkfs.ext2 -b 1024 -I 128 /dev/loop20p1
fi

# Mount loop device
mkdir /mnt/hdd
mount /dev/loop20p1 /mnt/hdd

# Copy files
cp hdd/* /mnt/hdd

# Clean up
umount /mnt/hdd
losetup -d /dev/loop20
