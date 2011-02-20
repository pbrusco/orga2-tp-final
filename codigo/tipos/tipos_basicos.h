#ifndef __TIPOS_BASICOS__
#define __TIPOS_BASICOS__

/*TIPOS BASICOS Y DEFINICIONES UTILES*/

#define NULL    0
#define TRUE    1
#define FALSE   0
#define OK		0

#define MB 0x100000
#define KB 0x400
#define TAM_PAG 0x1000



#define DIR_DIRECTORIO 0x100000
#define CANT_TAREAS 50

#define BASIC_EFLAGS 0x202

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long qword;

//booleanos
#define true 1
#define false 0




//definicion de estados posibles para las tareas en la BCP
#define MUERTO 0
#define CORRIENDO 1
#define ACTIVO 2
#define DORMIDO 3
#define MATAR 4
#define KERNEL -1



//definicion de funciones en assembler inline
#define breakpoint() __asm__ __volatile__("xchg %bx, %bx")




#endif
