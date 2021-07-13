GCCPARAMS="-m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -Ofast"
ASPARAMS="-f elf"
LDPARAMS="-melf_i386"

# Build bootloader
nasm $ASPARAMS boot/boot.asm -o boot/boot.o

# Build drivers
gcc $GCCPARAMS -c drivers/vgacon.c -o drivers/vgacon.o
gcc $GCCPARAMS -c drivers/rtc.c -o drivers/rtc.o
gcc $GCCPARAMS -c drivers/pci.c -o drivers/pci.o
gcc $GCCPARAMS -c drivers/serial.c -o drivers/serial.o
gcc $GCCPARAMS -c drivers/keyboard.c -o drivers/keyboard.o
gcc $GCCPARAMS -c drivers/mouse.c -o drivers/mouse.o
gcc $GCCPARAMS -c drivers/pit.c -o drivers/pit.o
gcc $GCCPARAMS -c drivers/ac97.c -o drivers/ac97.o
gcc $GCCPARAMS -c drivers/ata.c -o drivers/ata.o
gcc $GCCPARAMS -c drivers/rtl8139.c -o drivers/rtl8139.o

# Build libc
#gcc $GCCPARAMS -c libc/string/memcmp.c -o libc/string/memcmp.o
#gcc $GCCPARAMS -c libc/string/memcpy.c -o libc/string/memcpy.o
#gcc $GCCPARAMS -c libc/string/memmove.c -o libc/string/memmove.o
#gcc $GCCPARAMS -c libc/string/memset.c -o libc/string/memset.o
#gcc $GCCPARAMS -c libc/string/strlen.c -o libc/string/strlen.o
gcc $GCCPARAMS -c libc/string.c -o libc/string.o
gcc $GCCPARAMS -c libc/mm.c -o libc/mm.o

# Build lib
gcc $GCCPARAMS -c lib/libbmp.c -o lib/libbmp.o

gcc $GCCPARAMS -c lib/gui/direct.c -o lib/gui/direct.o
gcc $GCCPARAMS -c lib/gui/window.c -o lib/gui/window.o

# Build kernel
gcc $GCCPARAMS -c kernel/debug.c -o kernel/debug.o
gcc $GCCPARAMS -c kernel/io.c -o kernel/io.o
gcc $GCCPARAMS -c kernel/gdt.c -o kernel/gdt.o
gcc $GCCPARAMS -c kernel/idt.c -o kernel/idt.o
gcc $GCCPARAMS -c kernel/irq.c -o kernel/irq.o
gcc $GCCPARAMS -c kernel/platform/platform.c -o kernel/platform/platform.o
gcc $GCCPARAMS -c kernel/platform/smbios.c -o kernel/platform/smbios.o
gcc $GCCPARAMS -c kernel/panic.c -o kernel/panic.o
gcc $GCCPARAMS -c kernel/mem/pmm.c -o kernel/mem/pmm.o
gcc $GCCPARAMS -c kernel/net/ethernet.c -o kernel/net/ethernet.o
gcc $GCCPARAMS -c kernel/net/arp.c -o kernel/net/arp.o
gcc $GCCPARAMS -c kernel/net/ip.c -o kernel/net/ip.o
nasm $ASPARAMS kernel/idt.asm -o kernel/idt_asm.o
nasm $ASPARAMS kernel/gdt.asm -o kernel/gdt_asm.o

gcc $GCCPARAMS -c kernel/fs/vfs.c -o kernel/fs/vfs.o
gcc $GCCPARAMS -c kernel/fs/ext2.c -o kernel/fs/ext2.o

gcc $GCCPARAMS -c init/main.c -o init/main.o

# Build programs
#gcc $GCCPARAMS -c apps/shell/shell.c -o bin/shell/shell.o
gcc $GCCPARAMS -c apps/desktop/desktop.c -o apps/desktop/desktop.o

#libc/string/memcmp.o libc/string/memcpy.o libc/string/memmove.o libc/string/memset.o libc/string/strlen.o libc/mm.o \

# Linking
ld $LDPARAMS -T linker.ld -o myos.bin kernel/debug.o kernel/io.o kernel/gdt.o kernel/idt.o kernel/irq.o kernel/platform/platform.o kernel/platform/smbios.o kernel/panic.o kernel/mem/pmm.o kernel/idt_asm.o kernel/gdt_asm.o kernel/fs/vfs.o kernel/fs/ext2.o kernel/net/ethernet.o kernel/net/arp.o kernel/net/ip.o \
boot/boot.o \
init/main.o \
drivers/vgacon.o drivers/rtc.o drivers/pci.o drivers/serial.o drivers/keyboard.o drivers/mouse.o drivers/pit.o drivers/ac97.o drivers/ata.o drivers/rtl8139.o \
libc/string.o libc/mm.o \
apps/desktop/desktop.o \
lib/libbmp.o lib/gui/direct.o lib/gui/window.o

mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
