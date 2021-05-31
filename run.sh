# ATA
#qemu-system-i386 -boot d -cdrom myos.iso -hda hdd.img

# ATA + RTL8139
qemu-system-i386 -boot d -cdrom myos.iso -hda hdd.img -nic user,model=rtl8139,mac=76:36:E8:9F:64:6D


