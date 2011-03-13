#ifndef __BCP__
#define __BCP__


typedef struct BCP_Entry_s{
	uint16 pid;			//indice de la tarea en el gdt_vector
	uint8 estado;			//indica el estado de la tarea
	uint32 *entrada_directorio;	//direccion del directorio de la tarea
	uint8 sig;			//siguiente tarea para el round robin scheduler
	uint8 ant;			//anterior tarea para el round robin scheduler
	uint16 *pantalla;			//puntero a la pagina destinada al video de la tarea
	int8* nombre;
	uint32* dir_fisica;
} __attribute__((__packed__, aligned (8))) BCP_Entry;


/* Funciones */

// llena el BPC[0] con los datos del kernel, y inicializa variables globales
void iniciar_BCP();

// busca entrada libre en el BCP (libre <=> estado == muerto)
uint16 buscar_entradaBCP_vacia();

// llena la entrada con los datos de la tarea y la agrega al final de la cola de tareas activas
void crear_entradaBCP(uint32 pid, uint8 estado, uint32* entrada_directorio, uint16 *pantalla, int8* name, uint32* dir_fisica);

// cambia el estado de una tarea, y si el estado es MUERTO la quita de la cola de tareas activas
void cambiar_estado(uint16 bcpPos, uint8 estado_nuevo);

// devuelve la posicion en la BCP de la tarea "id". Recordar que "id" es la posicion de la entrada tss de la tarea en la gdt
uint8 buscar_entradaBCP(uint16 id);

// devuelve la posicion en la BCP de alguna tarea con estado "MUERTA". Si no hay ninguna, devuelve CANT_TAREAS
uint8 buscar_entradaBCP_matar();

// Sirve para debuggear. Dada una entrada de BCP, imprime por pantalla todo su contenido
void info_BCP(uint8 index);

// carga una tarea y todo sus datos y contexto en memoria y la agrega en la BCP para incluirla en el scheduling
uint16 cargarTarea(uint32 eip, uint32 tam, int8* name);

// Marcar tarea como "MATAR" para que luego el KERNEL se encargue de eliminarla.
// El param "id" es la posicion en la GDT del segmento TSS de la tarea que se quiere eliminar.
void matarTarea(uint8 id);


void kill_app(uint16 bcpPos);

//esta tarea se la llama cuando la tarea actual quiere terminar y llama a la interrupcion 80
void exit();

/*Esta funcion se va a llamar cada vez que se ejecute el kernel. La idea es que si hay alguna tarea en la BCP marcada como "MATAR" (ya va a estar fuera del scheduler), esta funcion se encargue de eliminar y liberar todas las estructuras utilizadas por la tarea (BCP, TSS, directorio y tablas de páginas, paginas de video y de pila y gdt).
Recibe como parametro la entrada de BCP de la tarea.
*/
void desaparecerTarea(uint8 );

// mapea las paginas por default para una tarea (paginas de la gdt,idt,tss,y demas estructuras)
void mapeo_paginas_default(uint32* directorio);

#endif

