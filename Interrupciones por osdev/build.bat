@echo off

echo ----------===============IDT Kernel Builder==============----------

echo [bootsector...]
nasm -f bin boot.asm
echo [kernel init...]
nasm -f aout k_init.asm
echo [interrupt handlers..]
nasm -f aout interrupts.asm

echo [kernel main...]
gcc -fno-builtins -c *.c

echo [linking all...]
ld -T kernel.ld -o kernel.bin k_init.o kernel.o stdio.o io.o pic.o idt.o panic.o interrupts.o

copy /b boot+kernel.bin kernel.img > nul

del *.o
del kernel.bin

echo use kernel.img to boot the kernel

echo [Done!]