GCCPARAMS="-m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings"
ASPARAMS="--32"
LDPARAMS="-melf_i386"

as $ASPARAMS boot.s -o boot.o

gcc $GCCPARAMS -c kernel.c -o kernel.o
#gcc $GCCPARAMS -c tty.c -o tty.o

ld $LDPARAMS -T linker.ld -o myos.bin boot.o kernel.o

mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
