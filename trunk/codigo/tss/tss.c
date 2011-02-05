
#include "tss.h"
#include "../gdt/gdt.h"

// Arreglo de TSSs, asi tenemos todo mas ordenado
tss TSS[CANT_TAREAS];


void crear_TSS(tss* entry, dword CR3, dword EIP, dword EFLAGS){
	entry->cr3 = CR3;
	entry->eflags = EFLAGS;
	entry->eip = EIP;
	entry->ptl = 0xFE0;
}



tss* buscar_TSS_vacia(){
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
		return NULL;
	else
		return &TSS[res];
}


void vaciar_TSS(byte n){
	TSS[n].cr3 = 0;
}
