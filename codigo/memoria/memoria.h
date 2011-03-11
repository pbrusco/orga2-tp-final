#ifndef __MM__
#define __MM__

/* MEMORIA BASICA */
/*================*/


/* Constantes */
/*------------*/
#define DIR_KERNEL_END 0x200000 	// Hasta los 2 MB reservados para el kernel.
#define DIR_BASE_MALLOC 0XE7000000


/* Variables globales */
/*--------------------*/
uint32 memoria_total;		// Tamaño en MB de toda la memoria del sistema.
uint32 paginas_libres;		// Cantidad de páginas libres en la memoria del sistema.
uint8 *dir_init_bitmap;		// Puntero al inicio del Bitmap de memoria.
uint8 *dir_end_bitmap;		// Puntero a la ultima dirección válida del Bitmap de memoria.


/* Funciones */
/*-----------*/

//~ Cuenta la cantidad de MB de memoria en el sistema y registra la cantidad de páginas libres.
void contarMemoria();

//~ Inicializa el Bitmap de memoria marcando como ocupadas las páginas utilizadas por el kernel.
void llenarBitmap();

//~ Devuelve un puntero a una pagina libre, y la marca como ocupada en el Bitmap.
uint32* pidoPagina();

//~ Dada un puntero a una pagina, la marca como libera dentro del Bitmap.
void liberoPagina(uint32 *);

//~ Setea las posiciones de memoria desde "dir" hasta "dir"+"cant" con lo indicado en set.
void setmem(uint8* dir, uint8 set, uint32 cant);

// Copia "cant" byetes desde la posición de memoria "from" hacia la posiciób "to".
void cpmem(uint8* from, uint8* to, uint32 cant);

















/*APROX MAPA DE MEMORIA DEL KERNEL*/
void donde_esta_el_kernel();
/*
 0X1200 - 0X1FFF = kernel.asm con gdt_vector
 
 0x2000 - 0xCFFF = reservado para tareas
 
 0xD000 - 0xDFFF = make_descriptor | pidoPagina | mapear_pagina | crear_TSS
 
 0xE000 - 0xEFFF = idtFill | _isr0-21
 
 0xF000 - 0xFFFF = console | extraerNumero | printf | switch_task | getChar
 
 0x10000- 0x10FFF = levanto | tarea_actual | idt[] | IDT_DESC | paginas_libres | puntero_pantalla | teclado | TSS[0]
 
 0x

*/



/* MEMORIA AVANZADA (es un artilugio mucho muy complejo) */

typedef uint32 Entry_Heap;

typedef struct Heap_s{
	uint32 bytes_disponibles;
	Entry_Heap* init; 
	Entry_Heap* end;
} __attribute__((__packed__, aligned (8))) Heap;


/* Crea un nuevo Heap en la direccion heap_dir, de tamaño bytes. Las direcciones init y end no se pasan ya que al estar iniciando la estructura, deberian ser iguales, y corresponden a la direccion siguiente a la de heap_dir (ojo!).
Debería ejecutarse la primera vez que una tarea hace un malloc, en cuyo caso, la rutina que la llama sería la encargada de ejecutar la syscall del malloc.

Ponemos un limite para la cantidad de bytes maximos que se le dan a un proceso? 2% de la memoria total les parece?

*/
void crear_heap(Heap* heap_dir, uint32 cant_bytes);


/*
Precondicion: ya debe existir un Heap.
Si hay algún error o no hay suficiente espacio, devuelve 0.
cant_bytes no puede exceder los 2 GB (2^31 - 1 como maximo)
*/
void* pedir_memoria(Heap* heap_dir, uint32 cant_bytes);

//auxiliar de "pedir_memoria"
Entry_Heap* devolverDondeEntre(Heap* heap_dir, uint32 cant_bytes);


/* Precondicion: ya debe existir un Heap.
Libera la memoria utilizada en dir (si es que estaba en uso) y une los espacios contiguos libres en la estructura que aparecen al liberar dir.
*/
void liberar_memoria(Heap* heap_dir, void* dir);


#endif
