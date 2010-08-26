#include "bcp.h"


//llena el BPC[0] con los datos del kernel, y inicializa variables globales
void iniciar_BCP(){

	//variables globales
	*tarea_actual = BCP;//la tarea BCP[0] es la del kernel
	cant_tareas_en_sistema = 1;

	

}


void crear_entrada(dword pid, byte estado, dword* entrada_directorio, BCP_Entry* ant, BCP_Entry* sig){

	
	BCP_Entry* nuevo = BCP;

	//buscar entrada MUERTA y poner en "nuevo"

	*nuevo = (BCP_Entry){
			(dword) pid,
			(byte) estado,
			
		;}


} 



void eliminar_entrada(dword pid);
void cambiar_estado(dword pid, byte estado_nuevo);