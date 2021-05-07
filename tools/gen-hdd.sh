#!/bin/sh

block_size=512
blocks=51200
output_file="hdd.img"
fs_type="fat"
fs_code="6"

fat_type="16"
fat_name="HDD"
fat_secs_per_cluster=2
fat_root_entries=512

# Create raw image
dd if=/dev/zero of=$output_file bs=$block_size count=$blocks

# Setup mbr and first partition
(
echo o # Create a new empty DOS partition table
echo n # Add a new partition
echo p # Primary partition
echo 1 # Partition number
echo   # First sector (Accept default: 1)
echo   # Last sector (Accept default: varies)
echo t # Set partition type
echo $fs_code
echo w # Write changes
) | fdisk $output_file

# Setup loop device
losetup -f -P $output_file

if [ $fs_type = "fat" ]; then
        mkfs.fat -F $fat_type -n $fat_name -s $fat_secs_per_cluster -r $fat_root_entries /dev/loop0p1
fi

# Mount hdd
mount /dev/loop0p1 /mnt
