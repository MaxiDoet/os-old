QEMU=/home/dev/qemu/build/qemu-system-i386

#$QEMU -boot d -cdrom myos.iso -hda hdd.img -machine type=q35
#$QEMU -boot d -cdrom myos.iso -hda hdd.img -machine type=q35 -nic user,model=rtl8139
$QEMU -boot d -cdrom myos.iso -hda hdd.img -machine type=q35 -device rtl8139,netdev=net0 -netdev user,id=net0,hostfwd=tcp::5555-:22
#$QEMU -boot d -cdrom myos.iso -hda hdd.img -device ich9-intel-hda -device hda-micro -trace enable=hda_audio_running
#$QEMU -boot d -cdrom myos.iso -hda hdd.img -machine type=q35 -device AC97
