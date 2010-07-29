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


#define DIR_DIRECTORIO 0x100000 //1MB

#define PRESENT 	1 << 0
#define READ 		0 << 1
#define WRITE 		1 << 1
#define SUPERVISOR	0 << 2
#define USUARIO		1 << 2

void mapear_directorio(dword *dir_entry, dword dir_table, word atributos);

void iniciar_paginacion_kernel();



#endif
