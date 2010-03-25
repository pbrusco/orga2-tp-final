#include "tss.h"

#define TSS_COUNT 50

//Arreglo de TSSs 
tss tsss[TSS_COUNT] = {
	
	// Tarea Nula
	(tss) {
		(unsigned short) 0,
		(unsigned short) 0,
		(unsigned int) 0,
		(unsigned short) 0,
		(unsigned short) 0,
		(unsigned int) 0,
		(unsigned short) 0,
		(unsigned short)  0,
		(unsigned int) 0,
		(unsigned short) 0,
		(unsigned short) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned int) 0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0,
		(unsigned short)  0
	},
	
	// Tarea 1 - Pintor
	(tss) {
		(unsigned short) 0, //ptl;
		(unsigned short) 0, //unused0;
		(unsigned int) 0, //esp0;
		(unsigned short)  0, //ss0;
		(unsigned short)  0, //unused1;
		(unsigned int) 0, //esp1;
		(unsigned short)  0, //ss1;
		(unsigned short)  0, //unused2;
		(unsigned int) 0, //esp2;
		(unsigned short)  0, //ss2;
		(unsigned short)  0, //unused3;
		(unsigned int) 0xA000, //cr3;
		(unsigned int) 0x8000, //eip;
		(unsigned int) 0x202, //eflags;
		(unsigned int) 0, //eax;
		(unsigned int) 0, //ecx;
		(unsigned int) 0, //edx;
		(unsigned int) 0, //ebx;
		(unsigned int) 0x15FFF, //esp;
		(unsigned int) 0x15FFF, //ebp;
		(unsigned int) 0, //esi;
		(unsigned int) 0, //edi;
		(unsigned short)  0x10, //es;
		(unsigned short)  0, //unused4;
		(unsigned short)  0x8, //cs;
		(unsigned short)  0, //unused5;
		(unsigned short)  0x10, //ss;
		(unsigned short)  0, //unused6;
		(unsigned short)  0x10, //ds;
		(unsigned short)  0, //unused7;
		(unsigned short)  0x10, //fs;
		(unsigned short)  0, //unused8;
		(unsigned short)  0x10, //gs;
		(unsigned short)  0, //unused9;
		(unsigned short)  0, //ldt;
		(unsigned short)  0, //unused10;
		(unsigned short)  0, //dtrap;
		(unsigned short)  0, //iomap;
	},

	// Tarea 2 - Traductor
	(tss) {
		(unsigned short) 0, //ptl;
		(unsigned short) 0, //unused0;
		(unsigned int) 0, //esp0;
		(unsigned short)  0, //ss0;
		(unsigned short)  0, //unused1;
		(unsigned int) 0, //esp1;
		(unsigned short)  0, //ss1;
		(unsigned short)  0, //unused2;
		(unsigned int) 0, //esp2;
		(unsigned short)  0, //ss2;
		(unsigned short)  0, //unused3;
		(unsigned int) 0xB000, //cr3;
		(unsigned int) 0x9000, //eip;
		(unsigned int) 0x202, //eflags;
		(unsigned int) 0, //eax;
		(unsigned int) 0, //ecx;
		(unsigned int) 0, //edx;
		(unsigned int) 0, //ebx;
		(unsigned int) 0x16FFF, //esp;
		(unsigned int) 0x16FFF, //ebp;
		(unsigned int) 0, //esi;
		(unsigned int) 0, //edi;
		(unsigned short)  0x10, //es;
		(unsigned short)  0, //unused4;
		(unsigned short)  0x8, //cs;
		(unsigned short)  0, //unused5;
		(unsigned short)  0x10, //ss;
		(unsigned short)  0, //unused6;
		(unsigned short)  0x10, //ds;
		(unsigned short)  0, //unused7;
		(unsigned short)  0x10, //fs;
		(unsigned short)  0, //unused8;
		(unsigned short)  0x10, //gs;
		(unsigned short)  0, //unused9;
		(unsigned short)  0, //ldt;
		(unsigned short)  0, //unused10;
		(unsigned short)  0, //dtrap;
		(unsigned short)  0, //iomap;
	}


};
