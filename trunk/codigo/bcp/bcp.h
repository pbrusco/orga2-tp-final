#ifndef __BCP__
#define __BCP__

#include "../tipos/tipos_basicos.h"


typedef struct BCP_Entry_s{
	word pid;
	byte estado;
	dword *entrada_directorio;
	struct BCP_Entry_s *sig;
	struct BCP_Entry_s *ant;
	byte reservado;
} __attribute__((__packed__, aligned (8))) BCP_Entry;

BCP_Entry BCP[CANT_TAREAS];

//variables globales
BCP_Entry* tarea_actual;
word cant_tareas_en_sistema;


/* Funciones */

//llena el BPC[0] con los datos del kernel, y inicializa variables globales
void iniciar_BCP(); 

// busca entrada libre en el BCP (libre <=> estado == muerto)
word buscar_entradaBCP_vacia();

// llena la entrada con los datos de la tarea y la agrega al final de la cola de tareas activas
void crear_entrada(word entrada, dword pid, byte estado, dword* entrada_directorio);

// cambia el estado de una tarea, y si el estado es MUERTO la quita de la cola de tareas activas
void cambiar_estado(dword pid, byte estado_nuevo);

// devuelve el pid de la proxima tarea en la cola
word proxima_tarea();


#endif 