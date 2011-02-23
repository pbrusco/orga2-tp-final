#ifndef __MM__
#define __MM__

/* MEMORIA BASICA */

//variables globales

dword memoria_total;//en MB
byte *dir_init_bitmap;//debajo de las tablas de paginas del kernel
byte *dir_end_bitmap;//ultima dir valida del bitmap
dword paginas_libres;


#define DIR_KERNEL_END 0x200000 //hasta los 2 MB reservados para el kernel
#define DIR_BASE_MALLOC 0XE7000000


// funciones

void contarMemoria();

void llenarBitmap();
dword* pidoPagina();
void liberoPagina(dword *);

// setea las posiciones de memoria desde dir hasta dir+cant con lo indicado en set
void setmem(byte* dir, byte set, dword cant);

// copia el contenido de la memoria de from a to, la cantidad de bytes que indique cant
void cpmem(byte* from, byte* to, dword cant);



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

typedef dword Entry_Heap;

typedef struct Heap_s{
	dword bytes_disponibles;
	Entry_Heap* init; 
	Entry_Heap* end;
} __attribute__((__packed__, aligned (8))) Heap;


/* Crea un nuevo Heap en la direccion heap_dir, de tamaño bytes. Las direcciones init y end no se pasan ya que al estar iniciando la estructura, deberian ser iguales, y corresponden a la direccion siguiente a la de heap_dir (ojo!).
Debería ejecutarse la primera vez que una tarea hace un malloc, en cuyo caso, la rutina que la llama sería la encargada de ejecutar la syscall del malloc.

Ponemos un limite para la cantidad de bytes maximos que se le dan a un proceso? 2% de la memoria total les parece?

*/
void crear_heap(Heap* heap_dir, dword cant_bytes);


/*
Precondicion: ya debe existir un Heap.
Si hay algún error o no hay suficiente espacio, devuelve 0.
cant_bytes no puede exceder los 2 GB (2^31 - 1 como maximo)
*/
void* pedir_memoria(Heap* heap_dir, dword cant_bytes);

//auxiliar de "pedir_memoria"
Entry_Heap* devolverDondeEntre(Heap* heap_dir, dword cant_bytes);


/* Precondicion: ya debe existir un Heap.
Libera la memoria utilizada en dir (si es que estaba en uso) y une los espacios contiguos libres en la estructura que aparecen al liberar dir.
*/
void liberar_memoria(Heap* heap_dir, void* dir);



#endif
