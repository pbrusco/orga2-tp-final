#include "bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"

extern tss TSS;
/*
//llena el BPC[0] con los datos del kernel, y inicializa variables globales
void iniciar_BCP(){

	//variables globales
	tarea_actual = BCP;//la tarea BCP[0] es la del kernel
	cant_tareas_en_sistema = 1;
	
	//BCP del kernel
	gdt[5] = make_descriptor(0, 0, 0, 0);
	
	
	BCP[0].pid = 0x28;//entrada TSS en la GDT del kernel

	

}


void crear_entrada(dword id, byte estado, dword* entrada_directorio, BCP_Entry* ant, BCP_Entry* sig){

	
	BCP_Entry* nuevo = BCP;

	//buscar entrada MUERTA y poner en "nuevo"

	nuevo->pid = id;


} 



void eliminar_entrada(dword pid){}
void cambiar_estado(dword pid, byte estado_nuevo){}*/
