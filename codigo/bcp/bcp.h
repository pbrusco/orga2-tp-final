#ifndef __BCP__
#define __BCP__

#include "../tipos/tipos_basicos.h"


//definicion de estados posibles
#define MUERTO 1
#define ACTIVO 2
#define DORMIDO 3


typedef struct BCP_Entry_s{
	word pid;
	byte estado;
	dword* entrada_directorio;
	BCP_Entry *sig;
	BCP_Entry *ant;
	byte reservado;
} __attribute__((__packed__, aligned (8))) BCP_Entry;

BCP_Entry BCP[CANT_TAREAS];

//variables globales
BCP_Entry* tarea_actual;
word cant_tareas_en_sistema;


//funciones

void iniciar_BCP(); //llena el BPC[0] con los datos del kernel, y inicializa variables globales

void crear_entrada(dword pid, byte estado, dword* entrada_directorio, BCP_Entry* ant, BCP_Entry* sig);
void eliminar_entrada(dword pid);
void cambiar_estado(dword pid, byte estado_nuevo);



#endif 
