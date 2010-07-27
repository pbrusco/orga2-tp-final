MCOPY=mcopy

KERNEL_SRC=init.asm
KERNEL_OBJ=init.o
KERNEL_BIN=kernel.bin


OBJ= $(KERNEL_OBJ)  gdt.o pic.o idt.o isr.o tss.o
DISK_IMG=diskette.img

CFLAGS=-m32 -g -ggdb -Wall -Werror -O0 \
  -fno-zero-initialized-in-bss -fno-stack-protector -ffreestanding

TASKS=pintor.tsk traductor.tsk

OBJDUMP=objdump
OBJCOPY=objcopy
CC=gcc
NASM=nasm
NASMFLAGS=-felf32
LD=ld
LDFLAGS=-static -m elf_i386 --oformat binary -b elf32-i386 -e start -Ttext 0x1200
.PHONY=clean all image 

all: cleantasks tasks clean image

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ $^

kernel.bin: $(OBJ)
	@echo 'Linkeando el kernel...'
	$(LD) $(LDFLAGS) -o $@ $^
	@echo ''


image: kernel.bin
	@echo 'Copiando el $(KERNEL_BIN) a la imagen de diskette\n'
	$(MCOPY) -i $(DISK_IMG) $(KERNEL_BIN) ::/ 

clean:
	rm -f *.bin
	rm -f *.o



%.tsk: %.asm
	$(NASM) -fbin -o $@ $^

tasks: $(TASKS)

cleantasks:
	rm -f *.tsk
