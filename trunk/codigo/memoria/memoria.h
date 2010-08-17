#ifndef __MM__
#define __MM__

/* MEMORIA BASICA */

//variables globales

dword memoria_total;//en MB
byte *dir_init_bitmap;//debajo de las tablas de paginas del kernel
byte *dir_end_bitmap;//ultima dir valida del bitmap
dword paginas_libres;


#define DIR_KERNEL_END 0x200000 //hasta los 2 MB reservados para el kernel
#define DIRECCION_BASE_MALLOC 0XE7000000


// funciones

void contarMemoria();

void llenarBitmap();
dword* pidoPagina();
void liberoPagina(dword *);


/* MEMORIA AVANZADA (es un artilugio mucho muy complejo) */




#endif
