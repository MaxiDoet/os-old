CC = gcc
CFLAGS = -m32 -Iinclude -Iinclude/lib -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -Ofast -c
AS = nasm
ASFLAGS = -f elf
LD = ld
LDFLAGS = -melf_i386

QEMU = /home/dev/qemu/build/qemu-system-i386

QEMUFLAGS = -boot d -hda hdd.img -machine type=q35 -device rtl8139,netdev=net0 -netdev user,id=net0,hostfwd=tcp::5555-:22
#QEMUFLAGS = -boot d -hda hdd.img -machine type=q35 -device AC97
#QEMUFLAGS = -boot d -hda hdd.img -device ich9-intel-hda -device hda-micro -trace enable=hda_audio_running

obj_files := boot/boot.o lib/gui/core/direct.o lib/gui/window.o ./libc/mm.o ./libc/string.o ./kernel/fs/vfs.o ./kernel/fs/ext2.o ./kernel/int_handler.o ./kernel/irq.o ./kernel/net/ethernet.o ./kernel/net/arp.o ./kernel/net/ip.o ./kernel/platform/platform.o ./kernel/platform/smbios.o ./kernel/io.o ./kernel/debug.o ./kernel/idt.o ./kernel/panic.o ./kernel/gdt.o ./kernel/mem/pmm.o ./kernel/gdt_setup.o ./apps/desktop/desktop.o ./drivers/keyboard.o ./drivers/mouse.o ./drivers/vgacon.o ./drivers/hda.o ./drivers/pci.o ./drivers/ata.o ./drivers/rtl8139.o ./drivers/ac97.o ./drivers/rtc.o ./drivers/pit.o ./drivers/serial.o ./init/main.o

all: os.iso

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

os.bin: $(obj_files)
	$(LD) -T linker.ld $(LDFLAGS) $^ -o os.bin

os.iso: os.bin
	mkdir -p isodir/boot/grub
	cp os.bin isodir/boot/os.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o os.iso isodir

clean:
	rm -f $(obj_files)
	rm -f *.iso*
	rm -fr isodir
	rm -f os.bin

run: os.iso
	$(QEMU) $(QEMUFLAGS) -cdrom os.iso
