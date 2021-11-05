QEMU=/home/dev/qemu/build/qemu-system-i386

#$QEMU -boot d -cdrom myos.iso -hda hdd.img -machine type=q35
#$QEMU -boot d -cdrom myos.iso -hda hdd.img -machine type=q35 -netdev user,id=ul -nic user,id=u1,model=rtl8139,mac=76:36:E8:9F:64:6D,hostfwd=tcp::5555-:22
$QEMU -boot d -cdrom myos.iso -hda hdd.img -device ich9-intel-hda -device hda-micro -trace enable=hda_audio_running
#$QEMU -boot d -cdrom myos.iso -hda hdd.img -machine type=q35 -device AC97
