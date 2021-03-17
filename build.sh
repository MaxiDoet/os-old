GCCPARAMS="-m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings"
ASPARAMS="--32"
LDPARAMS="-melf_i386"

# Build bootloader
as $ASPARAMS boot/boot.s -o boot/boot.o

# Build drivers
gcc $GCCPARAMS -c drivers/vga.c -o drivers/vga.o
gcc $GCCPARAMS -c drivers/cmos.c -o drivers/cmos.o
gcc $GCCPARAMS -c drivers/pci.c -o drivers/pci.o
gcc $GCCPARAMS -c drivers/serial.c -o drivers/serial.o

# Build libs
gcc $GCCPARAMS -c lib/print.c -o lib/print.o

# Build kernel
gcc $GCCPARAMS -c kernel/asm.c -o kernel/asm.o

gcc $GCCPARAMS -c init/main.c -o init/main.o

# Linking
ld $LDPARAMS -T linker.ld -o myos.bin kernel/asm.o boot/boot.o init/main.o drivers/vga.o drivers/cmos.o drivers/pci.o drivers/serial.o lib/print.o

mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
