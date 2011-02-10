
#include "tss.h"
#include "../gdt/gdt.h"


void crear_TSS(byte pos, dword CR3, dword EIP, dword EFLAGS, dword pila){
	tss* entry = &TSS[pos];
	entry->cr3 = CR3;
	entry->eflags = EFLAGS;
	entry->eip = EIP;
	entry->cs = 0x8;
	entry->ss = 0x10;
	entry->ds = 0x10;
	entry->fs = 0x10;
	entry->gs = 0x10;
	entry->es = 0x10;
	entry->esp = pila;
	entry->ebp = pila;
}



byte buscar_TSS_vacia(){
	//voy a tomar como TSS vacia aquella cuyo cr3 sea igual a 0

	//recordar que devuelve algo igual o mayor que CANT_TAREAS si no hay ningun lugar disponible
	word res = CANT_TAREAS;
	word i = 0;
	
	while(i < CANT_TAREAS){
		if(TSS[i].cr3 == 0){
			res = i;
			i = CANT_TAREAS;
		}
		i++;	
	}
	
	if(res == CANT_TAREAS)
		return -1;
	else
		return res;
}


void vaciar_TSS(byte n){
	TSS[n].cr3 = 0;
}
