-----------------------------------------------------------------
|                     IDT Example Kernel                        |
|                          By Xsism                             |
|                          02-01-03                             |
|              Coded For Bona Fide OS Development               |
|        All Intellectual rights reserved Mr. Xsism 2003        |
-----------------------------------------------------------------
Building:
--------
   To build this kernel, just run build.bat in windows. If you are
not running windows, then you need the following:
     1.nasm
     2.gcc
     3.ld
**Note:Due to inproper A20 gate enabling code, this kernel did not
       work on a real PC, only in bochs. My fault, i didn't test it
       the real thing, lesson learned. Newer version of boot.asm has
       fix the problem. Sorry again for the trouble I caused :)
-------
Running:
-------
   To run this example kernel you need to do one of two things:
     1. Copy the kernel.img file to sector 1 of a floppy disk and boot
        from the floppy disk
     2. Run the kernel.img file in Bochs x86 emulator

------------
Comming Soon:
------------
   Presently under development is a full blown step by step OS Dev tutorial
being created by me, Mr. Xsism. Keep your eyes and ears set on osdev.neopages.net
to find out when it will be available. Also, don't forget, Bona Fide is on IRC,
too! irc.wyldryde.net @ #osdev look for me, osmaker and akira and we can help you
with just about anything. Until then, Mr. Xsism out :)