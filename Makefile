GCCPARAMS := -m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -Ofast
ASPARAMS := -f elf
LDPARAMS := -melf_i386

SRCDIRS := kernel drivers init libc lib apps
SRCFILES := $(shell find $(SRCDIRS) -type f -name "*.c")
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))

.PHONY: clean

all: build gen-hdd
build: boot/boot.o $(OBJFILES) myos.bin myos.iso

boot/boot.o: boot/boot.asm
	nasm $(ASPARAMS) $^ -o $@

$(OBJFILES): $(SRCFILES)
	gcc $(GCCPARAMS) -c $? -o $@

myos.bin: $(OBJFILES)
	ld $LDPARAMS -T linker.ld -o myos.bin $?

myos.iso: myos.bin
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

gen-hdd: tools/gen-hdd.sh
	sudo tools/gen-hdd.sh

clean:
	@rm -f $(OBJFILES) *.o
	@rm -f boot/boot.o
	@rm -f *.bin *.iso
