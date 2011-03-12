#ifndef __TSS_H__
#define __TSS_H__

// Task State Segment - Definicion de las TSSs.

// Definicion de una TSS
typedef struct str_tss {
	uint16 ptl;
	uint16  unused0;
	uint32 esp0;
	uint16  ss0;
	uint16  unused1;
	uint32 esp1;
	uint16  ss1;
	uint16  unused2;
	uint32 esp2;
	uint16  ss2;
	uint16  unused3;
	uint32 cr3;//265bytes
	uint32 eip;
	uint32 eflags;
	uint32 eax;
	uint32 ecx;
	uint32 edx;
	uint32 ebx;
	uint32 esp;
	uint32 ebp;//512bytes
	uint32 esi;
	uint32 edi;
	uint16  es;
	uint16  unused4;
	uint16  cs;
	uint16  unused5;
	uint16  ss;
	uint16  unused6;
	uint16  ds;
	uint16  unused7;
	uint16  fs;
	uint16  unused8;
	uint16  gs;
	uint16  unused9;//1KB
	uint16  ldt;
	uint16  unused10;
	uint16  dtrap;
	uint16  iomap;//1KB+64bytes
} __attribute__((__packed__, aligned (8))) Tss;


/* Funciones */
void info_TSS();
void crear_TSS(uint8 entrada, uint32 CR3, uint32 EIP, uint32 EFLAGS, uint32 pila, uint32);
uint8 buscar_TSS_vacia();
void vaciar_TSS(Tss*);

#endif //__TSS_H__

