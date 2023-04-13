CC = gcc
CFLAGS = -m32 -Iinclude -Iinclude/lib -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -Ofast -c
AS = nasm
ASFLAGS = -f elf
LD = ld
LDFLAGS = -melf_i386

#QEMU = qemu-system-i386
QEMU = ../qemu/build/qemu-system-i386

#QEMUFLAGS = -boot d
#QEMUFLAGS = -boot d -hda hdd.img -device rtl8139,netdev=net0 -netdev user,id=net0,hostfwd=tcp::5555-:22
QEMUFLAGS = -boot d -hda hdd.img -device AC97 -netdev user,id=u1,hostfwd=tcp:127.0.0.1:8000-:12 -device rtl8139,netdev=u1 -object filter-dump,id=f1,netdev=u1,file=dump.dat
#QEMUFLAGS = -boot d -hda hdd.img -device AC97
#QEMUFLAGS = -boot d -hda hdd.img -device ich9-intel-hda -device hda-micro -trace enable=hda_audio_running

obj_files := boot/boot.o lib/gui/direct.o lib/gui/utils.o ./libc/string.o ./kernel/fs/fs.o ./kernel/dd/dev.o ./kernel/fs/ext2.o ./kernel/audio/dev.o ./kernel/cpu/int_handler.o ./kernel/irq.o ./kernel/scheduler.o ./kernel/net/utils.o ./kernel/net/net.o ./kernel/net/ethernet.o ./kernel/net/arp.o ./kernel/net/udp.o ./kernel/net/tcp.o ./kernel/net/dhcp.o ./kernel/net/dns.o ./kernel/net/ip.o ./kernel/platform/platform.o ./kernel/platform/smbios.o ./kernel/io/io.o ./kernel/io/pci.o ./kernel/debug.o ./kernel/cpu/idt.o ./kernel/cpu/cpu.o ./kernel/panic.o ./kernel/cpu/gdt.o ./kernel/console.o ./kernel/mem/pmm.o ./kernel/mem/heap.o ./kernel/cpu/gdt_setup.o ./apps/desktop/desktop.o ./drivers/keyboard.o ./drivers/mouse.o ./drivers/hda.o ./drivers/ata.o ./drivers/rtl8139.o ./drivers/ac97.o ./drivers/rtc.o ./drivers/pit.o ./drivers/serial.o ./init/main.o

all: os.iso

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

os.bin: $(obj_files)
	$(LD) -T linker.ld $(LDFLAGS) $^ -o os.bin

os.iso: os.bin
	mkdir -p build/boot/grub
	cp os.bin build/boot/os.bin
	cp grub.cfg build/boot/grub/grub.cfg
	grub-mkrescue -o os.iso build

clean:
	rm -f $(obj_files)
	rm -f *.iso*
	rm -fr build
	rm -f os.bin

run: os.iso
	$(QEMU) $(QEMUFLAGS) -cdrom os.iso
