MCOPY=mcopy

KERNEL_SRC=kernel.asm
KERNEL_OBJ=kernel.o
KERNEL_BIN=kernel.bin


OBJ_MINIMOS= $(KERNEL_OBJ)  gdt/gdt.o memoria/memoria.o paginacion/paginacion.o tss/tss.o bcp/bcp.o interrupciones/idt.o interrupciones/isr.o pantalla/pantalla.o scheduler/scheduler.o applications/console.o teclado/teclado.o kernel/kernel.o

DISK_IMG=almacenamiento/diskette.img


CFLAGS=-m32 -g -ggdb -Wall -O0 \
  -fno-zero-initialized-in-bss -fno-stack-protector -ffreestanding #-Werror

OBJDUMP=objdump
OBJCOPY=objcopy
CC=gcc
NASM=nasm
NASMFLAGS=-felf32
LD=ld
LDFLAGS=-static -m elf_i386 --oformat binary -b elf32-i386 -e start -Ttext 0x1200
.PHONY=clean all image

all: tareas clean image

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ $^

kernel.bin: $(OBJ_MINIMOS) $(OBJ_AGREGADOS)
	@echo 'Linkeando el kernel...'
	$(LD) $(LDFLAGS) -o $@ $^
	@echo ''

tareas:
	$(MAKE) -C ./tareas\ en\ binario/

image: kernel.bin
	@echo 'Copiando el $(KERNEL_BIN) a la imagen de diskette\n'
	$(MCOPY) -i $(DISK_IMG) $(KERNEL_BIN) ::/ -o

clean:
	rm -f *.bin
	rm -fr *.o

