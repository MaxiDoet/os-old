GCCPARAMS="-m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings"
ASPARAMS="--32"
LDPARAMS="-melf_i386"

# Build bootloader
as $ASPARAMS boot/boot.s -o boot/boot.o

# Build drivers
gcc $GCCPARAMS -c drivers/vga.c -o drivers/vga.o
gcc $GCCPARAMS -c drivers/cmos.c -o drivers/cmos.o

# Build libs
gcc $GCCPARAMS -c lib/print.c -o lib/print.o


gcc $GCCPARAMS -c init/main.c -o init/main.o

ld $LDPARAMS -T linker.ld -o myos.bin boot/boot.o init/main.o drivers/vga.o drivers/cmos.o lib/print.o

mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
