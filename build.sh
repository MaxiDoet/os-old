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

# Build libs
gcc $GCCPARAMS -c lib/print.c -o lib/print.o
gcc $GCCPARAMS -c lib/string.c -o lib/string.o

# Build kernel
gcc $GCCPARAMS -c kernel/asm.c -o kernel/asm.o
gcc $GCCPARAMS -c kernel/gdt.c -o kernel/gdt.o
gcc $GCCPARAMS -c kernel/idt.c -o kernel/idt.o
nasm $ASPARAMS kernel/idt.asm -o kernel/idt_asm.o
nasm $ASPARAMS kernel/gdt.asm -o kernel/gdt_asm.o

gcc $GCCPARAMS -c init/main.c -o init/main.o

# Build programs
gcc $GCCPARAMS -c bin/shell.c -o bin/shell.o

# Linking
ld $LDPARAMS -T linker.ld -o myos.bin kernel/asm.o kernel/gdt.o kernel/idt.o kernel/gdt_asm.o kernel/idt_asm.o boot/boot.o init/main.o drivers/vgacon.o drivers/cmos.o drivers/pci.o drivers/serial.o drivers/bga.o drivers/keyboard.o lib/print.o lib/string.o bin/shell.o

mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
