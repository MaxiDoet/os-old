#!/bin/sh

block_size=2M
blocks=5
output_file="hdd.img"
fs_type="fat"

fat_type="16"
fat_name="HDD"
fat_secs_per_cluster=2
fat_root_entries=512

# Create raw image
dd if=/dev/zero of=$output_file bs=$block_size count=$blocks

if [ $fs_type = "fat" ]; then
        mkfs.fat -F $fat_type -n $fat_name -s $fat_secs_per_cluster -r $fat_root_entries $output_file
fi
