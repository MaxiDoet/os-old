GCCPARAMS="-m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings"
ASPARAMS="-f elf"
LDPARAMS="-melf_i386"

# Build bootloader
nasm $ASPARAMS boot/boot.asm -o boot/boot.o

# Build drivers
gcc $GCCPARAMS -c drivers/vgacon.c -o drivers/vgacon.o
gcc $GCCPARAMS -c drivers/cmos.c -o drivers/cmos.o
gcc $GCCPARAMS -c drivers/pci.c -o drivers/pci.o
gcc $GCCPARAMS -c drivers/serial.c -o drivers/serial.o
gcc $GCCPARAMS -c drivers/bga.c -o drivers/bga.o
gcc $GCCPARAMS -c drivers/keyboard.c -o drivers/keyboard.o
gcc $GCCPARAMS -c drivers/mouse.c -o drivers/mouse.o
gcc $GCCPARAMS -c drivers/sb16.c -o drivers/sb16.o
gcc $GCCPARAMS -c drivers/pit.c -o drivers/pit.o
gcc $GCCPARAMS -c drivers/ac97.c -o drivers/ac97.o
gcc $GCCPARAMS -c drivers/ata.c -o drivers/ata.o

# Build libc
gcc $GCCPARAMS -c libc/string/memcmp.c -o libc/string/memcmp.o
gcc $GCCPARAMS -c libc/string/memcpy.c -o libc/string/memcpy.o
gcc $GCCPARAMS -c libc/string/memmove.c -o libc/string/memmove.o
gcc $GCCPARAMS -c libc/string/memset.c -o libc/string/memset.o
gcc $GCCPARAMS -c libc/string/strlen.c -o libc/string/strlen.o
gcc $GCCPARAMS -c libc/mm.c -o libc/mm.o

# Build kernel
gcc $GCCPARAMS -c kernel/debug.c -o kernel/debug.o
gcc $GCCPARAMS -c kernel/asm.c -o kernel/asm.o
gcc $GCCPARAMS -c kernel/gdt.c -o kernel/gdt.o
gcc $GCCPARAMS -c kernel/idt.c -o kernel/idt.o
gcc $GCCPARAMS -c kernel/irq.c -o kernel/irq.o
gcc $GCCPARAMS -c kernel/panic.c -o kernel/panic.o
gcc $GCCPARAMS -c kernel/mem/pmm.c -o kernel/mem/pmm.o
nasm $ASPARAMS kernel/idt.asm -o kernel/idt_asm.o
nasm $ASPARAMS kernel/gdt.asm -o kernel/gdt_asm.o

gcc $GCCPARAMS -c kernel/fs/fat.c -o kernel/fs/fat.o

gcc $GCCPARAMS -c init/main.c -o init/main.o

# Build programs
#gcc $GCCPARAMS -c bin/shell/shell.c -o bin/shell/shell.o
gcc $GCCPARAMS -c bin/desktop/desktop.c -o bin/desktop/desktop.o

# Linking
ld $LDPARAMS -T linker.ld -o myos.bin kernel/debug.o kernel/asm.o kernel/gdt.o kernel/idt.o kernel/irq.o kernel/panic.o kernel/mem/pmm.o kernel/idt_asm.o kernel/gdt_asm.o kernel/fs/fat.o \
boot/boot.o \
init/main.o \
drivers/vgacon.o drivers/cmos.o drivers/pci.o drivers/serial.o drivers/bga.o drivers/keyboard.o drivers/mouse.o drivers/sb16.o drivers/pit.o drivers/ac97.o drivers/ata.o \
libc/string/memcmp.o libc/string/memcpy.o libc/string/memmove.o libc/string/memset.o libc/string/strlen.o libc/mm.o \
bin/desktop/desktop.o

mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
