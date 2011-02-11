#ifndef __BCP__
#define __BCP__

#include "../tipos/tipos_basicos.h"


typedef struct BCP_Entry_s{
	word pid;
	byte estado;
	dword *entrada_directorio;
	byte sig;
	byte ant;
	word* cursor;
} __attribute__((__packed__, aligned (8))) BCP_Entry; 


//variables globales
char tarea_actual;
byte cant_tareas_en_sistema;


/* Funciones */

// llena el BPC[0] con los datos del kernel, y inicializa variables globales
void iniciar_BCP(); 

// busca entrada libre en el BCP (libre <=> estado == muerto)
word buscar_entradaBCP_vacia();

// llena la entrada con los datos de la tarea y la agrega al final de la cola de tareas activas
void crear_entradaBCP(word entrada, dword pid, byte estado, dword* entrada_directorio);

// cambia el estado de una tarea, y si el estado es MUERTO la quita de la cola de tareas activas
void cambiar_estado(dword pid, byte estado_nuevo);

// devuelve el pid de la proxima tarea en la cola
word proxima_tarea();

// provoca el switch de tareas

/*
ESTO DE ABAJO SON PRUEBAS PARA VER SI CARGA UNA TAREA Y SWITCHEA BIEN
*/

void cargarTarea1();
void cargarTarea2();/*
void switchTarea1();
void switchTarea2();
*/
#endif 
