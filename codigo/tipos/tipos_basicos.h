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
#define CANT_TAREAS 256


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long qword;





//definicion de estados posibles para las tareas en la BCP
#define MUERTO 0
#define CORRIENDO 1
#define ACTIVO 2
#define DORMIDO 3
#define KERNEL -1









#endif
