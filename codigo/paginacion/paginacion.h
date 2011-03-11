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




#define PRESENT 	0x1
#define READ_PAGINACION	0x0
#define WRITE 		0x2
#define SUPERVISOR	0x0
#define USUARIO		0x4

void mapear_tabla (uint32 *directorio, uint32 dir_tabla, uint16 atributos);
void mapear_pagina(uint32 *directorio, uint32 dir_virtual, uint32 dir_real, uint16 atributos);

// Dado un directorio y una dirección virtual, devuelve a qué página física está mapeada y sus atributos
uint32 obtener_mapeo(uint32 *directorio, uint32 dir_virtual);

#define OFFSET_TABLA 0X400000
#define OFFSET_PAGINA 0X1000


void iniciar_paginacion_kernel();

//funcion que dada la direccion de un directorio pone en cero (o NO presente) todas las tablas y paginas mapeadas y libera las paginas utilizadas para contenerlas
void liberar_directorio(uint32* dir_entry);


#endif
