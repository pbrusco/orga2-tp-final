#ifndef __BCP__
#define __BCP__

#include "../tipos/tipos_basicos.h"


typedef struct BCP_Entry_s{
	word pid;					//indice de la tarea en el gdt_vector
	byte estado;				//indica el estado de la tarea
	dword *entrada_directorio;	//direccion del directorio de la tarea
	byte sig;					//siguiente tarea para el round robin scheduler
	byte ant;					//anterior tarea para el round robin scheduler
	word *pantalla;				//TODO: puntero a la pagina destinada al video de la tarea
} __attribute__((__packed__, aligned (8))) BCP_Entry; 


//variables globales
byte tarea_actual;
byte cant_tareas_en_sistema;


/* Funciones */

// llena el BPC[0] con los datos del kernel, y inicializa variables globales
void iniciar_BCP(); 

// busca entrada libre en el BCP (libre <=> estado == muerto)
word buscar_entradaBCP_vacia();

// llena la entrada con los datos de la tarea y la agrega al final de la cola de tareas activas
void crear_entradaBCP(dword pid, byte estado, dword* entrada_directorio, word *pantalla);

// cambia el estado de una tarea, y si el estado es MUERTO la quita de la cola de tareas activas
void cambiar_estado(word pid, byte estado_nuevo);

// devuelve la posicion en la BCP de la tarea "id"
byte buscar_entradaBCP(word id);

// carga una tarea y todo sus datos y contexto en memoria y la agrega en la BCP para incluirla en el scheduling
void cargarTarea(dword eip);

// TODO: marcar tarea como muerta para que luego el KERNEL se encargue de eliminarla
void borrarTarea(byte id);

// mapea las paginas por default para una tarea (paginas de la gdt,idt,tss,y demas estructuras)
void mapeo_paginas_default(dword* directorio);

#endif 
