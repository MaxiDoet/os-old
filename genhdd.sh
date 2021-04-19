# 40MB
dd if=/dev/zero of=hdd.img bs=1024k seek=256 count=0

parted hdd.img mklabel msdos

# Create loop device
losetup -d /dev/loop0
losetup -f hdd.img

mkfs.fat /dev/loop0

# Create fat32 partition
#sfdisk /dev/loop0 < hdd/lay.out

# Reload partitions
#partprobe

# Mount hdd
mkdir ./hdd/mnt
umount /dev/loop0
mount /dev/loop0 ./hdd/mnt

# Copy files
echo "eee" > test.txt
mv test.txt hdd/mnt/

# Unmount hdd
umount ./hdd/mnt/
rm -r ./hdd/mnt/
