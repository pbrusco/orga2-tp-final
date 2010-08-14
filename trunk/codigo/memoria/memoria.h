#ifndef __MM__
#define __MM__

//variable global
dword memoria_total;//en MB
byte *dir_init_bitmap;//debajo de las tablas de paginas del kernel

#define DIR_KERNEL_END 0x200000 //hasta los 2 MB reservados para el kernel
#define DIRECCION_BASE_MALLOC 0XE7000000

void contarMemoria();

void llenarBitmap();
dword* pidoPagina();
void liberoPagina(dword *);



#endif
