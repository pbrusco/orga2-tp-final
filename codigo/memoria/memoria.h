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


/* MEMORIA AVANZADA (es un artilugio mucho muy complejo) */

typedef struct Entry_Heap_s{
	word ocupado_tam;
} __attribute__((__packed__, aligned (8))) Entry_Heap;


typedef struct Heap_s{
	dword bytes_disponibles;
	Entry_Heap* init; 
	Entry_Heap* end;
} __attribute__((__packed__, aligned (8))) Heap;


/* Crea un nuevo Heap en la direccion heap_dir, de tamaño bytes. Las direcciones init y end no se pasan ya que al estar iniciando la estructura, deberian ser iguales, y corresponden a la direccion siguiente a la de heap_dir (ojo!).
Debería ejecutarse la primera vez que una tarea hace un malloc, en cuyo caso, la rutina que la llama sería la encargada de ejecutar la syscall del malloc.
*/
void crear_heap(Heap* heap_dir, dword bytes);

/*
Precondicion: ya debe existir un Heap.
Si hay algún error o no hay suficiente espacio, devuelve 0.
cant_bytes no puede exceder los 2 GB (2^31 - 1 como maximo)
*/
void* pedir_memoria(dword cant_bytes);

/* Precondicion: ya debe existir un Heap.
Libera la memoria utilizada en dir (si es que estaba en uso) y une los espacios contiguos libres en la estructura que aparecen al liberar dir.
*/
void liberar_memoria(void* dir);



#endif
