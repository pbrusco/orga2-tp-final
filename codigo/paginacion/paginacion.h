#ifndef __PAGINACION__
#define __PAGINACION__


/*
entrada de directorio:
31-12 = direccion de la tabla de pagina
11-8 = se ignora
7 = PAGE SIZE (con 0 apunta a pagina de 4k)
6 = se ignora
5 = accedida
4 = page-level cache disabled (con 0)
3 = page-level write through
2 = supervisor/usuario (0/1)
1 = read/write (0/1)
0 = presente (1)

entrada de tabla de pagina:
31-12 = direccion de la tabla de pagina
11-9 = se ignora
8 = global (dejar en 0)
7 = PAT (dejar en 0)
6 = sucio (si esta en 1 indica que se escribio ahi)
5 = accedida
4 = page-level cache disabled (con 0)
3 = page-level write through
2 = supervisor/usuario (0/1)
1 = read/write (0/1)
0 = presente (1)

*/




#define PRESENT 	1 << 0
#define READ_PAGINACION	0 << 1
#define WRITE 		1 << 1
#define SUPERVISOR	0 << 2
#define USUARIO		1 << 2

void mapear_tabla (dword *directorio, dword dir_tabla, word atributos);
void mapear_pagina(dword *directorio, dword dir_virtual, dword dir_real, word atributos);

#define OFFSET_TABLA 0X400000
#define OFFSET_PAGINA 0X1000


void iniciar_paginacion_kernel();

//TODO: funcion que dada la direccion de un directorio pone en cero (o NO presente) todas las tablas y paginas mapeadas y libera las paginas utilizadas para contenerlas



#endif
