# ATA
#qemu-system-i386 -boot d -cdrom myos.iso -hda hdd.img

# ATA + RTL8139 + Packet Monitoring
#qemu-system-i386 -boot d -cdrom myos.iso -hda hdd.img -nic user,id=u1,model=rtl8139,mac=76:36:E8:9F:64:6D,hostfwd=tcp::5555-:22 -object filter-dump,id=f1,netdev=u1,file=dump.dat

# ATA + RTL8139 + RTL8139 Debug + AC97
/mnt/qemu/build/i386-softmmu/qemu-system-i386 -boot d -cdrom myos.iso -hda hdd.img -nic user,id=u1,model=rtl8139,mac=76:36:E8:9F:64:6D,hostfwd=tcp::5555-:22

# ATA + RTL8139 + AC97
#/mnt/qemu/build/i386-softmmu/qemu-system-i386 -boot d -cdrom myos.iso -hda hdd.img -device AC97
