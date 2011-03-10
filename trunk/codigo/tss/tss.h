#ifndef __TSS_H__
#define __TSS_H__

// Task State Segment - Definicion de las TSSs.

// Definicion de una TSS 
typedef struct str_tss {
	unsigned short ptl;
	unsigned short  unused0;
	unsigned int esp0;
	unsigned short  ss0;
	unsigned short  unused1;
	unsigned int esp1;
	unsigned short  ss1;
	unsigned short  unused2;
	unsigned int esp2;
	unsigned short  ss2;
	unsigned short  unused3;
	unsigned int cr3;//265bytes
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;//512bytes
	unsigned int esi;
	unsigned int edi;
	unsigned short  es;
	unsigned short  unused4;
	unsigned short  cs;
	unsigned short  unused5;
	unsigned short  ss;
	unsigned short  unused6;
	unsigned short  ds;
	unsigned short  unused7;
	unsigned short  fs;
	unsigned short  unused8;
	unsigned short  gs;
	unsigned short  unused9;//1KB
	unsigned short  ldt;
	unsigned short  unused10;
	unsigned short  dtrap;
	unsigned short  iomap;//1KB+64bytes
} __attribute__((__packed__, aligned (8))) tss;


// Arreglo de TSSs
tss TSS[CANT_TAREAS];


/* Funciones */
void crear_TSS(byte entrada, dword CR3, dword EIP, dword EFLAGS, dword pila, dword);
byte buscar_TSS_vacia();
void vaciar_TSS(tss*);

#endif //__TSS_H__ 
