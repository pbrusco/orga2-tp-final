#include "bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"

extern tss TSS;

void iniciar_BCP(){

	//variables globales
	tarea_actual = BCP;//la tarea BCP[0] es la del kernel
	cant_tareas_en_sistema = 1;
	
	
	BCP[0].pid = buscar_entradaGDT_vacia();//entrada TSS en la GDT donde estara el kernel
	BCP[0].estado = KERNEL;
	BCP[0].entrada_directorio = DIR_DIRECTORIO;
	BCP[0].ant = &BCP[0];
	BCP[0].sig = &BCP[0];

}

void iniciar_tss_kernel(){
    word tssVacia = buscar_TSS_vacia();
    
    gdt[BCP[0].pid] = make_descriptor(&tss[tssVacia], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_0 | TSS_0_OBLIGATORIO , TSS_GRANULARIDAD |  AVAILABLE)
    
    
    
    
}



void crear_entrada(word entrada, dword id, byte estado, dword* ent_directorio){
	BCP_Entry* nuevo = &BCP[entrada];
	nuevo->pid = id;
	nuevo->estado = ACTIVO;
	nuevo->entrada_directorio = ent_directorio;
	nuevo->ant = tarea_actual->ant;
	nuevo->sig = tarea_actual;
	(tarea_actual->ant)->sig = nuevo;
	tarea_actual->ant = nuevo;
} 


word buscar_entradaBCP_vacia(){

	word vacia = 1;
	
	while(BCP[vacia].estado != MUERTO){
		vacia++;
	}

	return vacia;
}


void cambiar_estado(dword id, byte estado_nuevo){

	BCP_Entry *busca = tarea_actual;

	while(busca->pid != id){
		busca = busca->sig;
	}

	busca->estado = estado_nuevo;
	if(estado_nuevo == MUERTO){
		(busca->ant)->sig = busca->sig;
		(busca->sig)->ant = busca->ant;
	}
}
