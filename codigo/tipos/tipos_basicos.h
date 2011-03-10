#ifndef __TIPOS_BASICOS__
#define __TIPOS_BASICOS__

/*TIPOS BASICOS Y DEFINICIONES UTILES*/

#define NULL    0
#define TRUE    1
#define FALSE   0
#define OK	0

#define MB 0x100000
#define KB 0x400
#define TAM_PAG 0x1000



#define DIR_DIRECTORIO 0x100000
#define CANT_TAREAS 50

//FLAGS
#define USER_EFLAGS 0x3202
#define KERNEL_EFLAGS 0x202

//SELECTORES DE SEGMENTOS
#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_CS 0x18
#define USER_DS 0x20


//TIPOS BASICOS
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long qword;




//definicion de estados posibles para las tareas en la BCP
#define MUERTO 0
#define CORRIENDO 1
#define ACTIVO 2
#define DORMIDO 3
#define MATAR 4
#define KERNEL -1


/**************************************************
****definicion de funciones en assembler inline****
**************************************************/

#define breakpoint() __asm__ __volatile__("xchg %bx, %bx")
#define cli() __asm__ __volatile__ ("cli");
#define sti() __asm__ __volatile__ ("sti");

/*ESTAS DEFINICIONES SIRVEN PARA HACER SALIDA DE VALORES A PUERTOS*/
#define outb(p,v) __asm__ __volatile__("outb %%al, %%dx" : : "al"(v), "dx"(p) )
#define outw(p,v) __asm__ __volatile__("outw %%al, %%dx" : : "ax"(v), "dx"(p) )
#define outd(p,v) __asm__ __volatile__("outd %%al, %%dx" : : "eax"(v), "dx"(p) )

/*FUNCIONES PARA MANEJO DEL REGISTRO CR3*/
#define get_cr3(v) __asm__ __volatile__ ( "movl %%cr3, %0" : "=r"(v) )
#define set_cr3(v) __asm__ __volatile__ ( "movl %0, %%cr3" : : "r"(v) )



#endif
