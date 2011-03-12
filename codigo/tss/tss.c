#include "../tipos/tipos_basicos.h"
#include "tss.h"
#include "../gdt/gdt.h"
#include "../pantalla/pantalla.h"



// Arreglo de TSSs
Tss TSS[CANT_TAREAS];


void crear_TSS(uint8 pos, uint32 CR3, uint32 EIP, uint32 EFLAGS, uint32 pila, uint32 ESP0){
	Tss* entry = &TSS[pos];
	entry->cr3 = CR3;
	entry->eflags = EFLAGS;
	entry->eip = EIP;
	entry->cs = 0x18 | 3;
	entry->ss = 0x20 | 3;
	entry->ds = 0x20 | 3;
	entry->fs = 0x20 | 3;
	entry->gs = 0x20 | 3;
	entry->es = 0x20 | 3;
	entry->esp = pila;
	entry->ebp = pila;
	//TODO: ver el valor que debe ir en los selectores

	entry->ss0 = 0x10;
	entry->esp0 = ESP0;
	entry->iomap = 0xFFFF;
}


uint8 buscar_TSS_vacia(){
	//voy a tomar como TSS vacia aquella cuyo cr3 sea igual a 0

	//recordar que devuelve algo igual o mayor que CANT_TAREAS si no hay ningun lugar disponible
	uint16 res = CANT_TAREAS;
	uint16 i = 0;

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


void info_TSS()
{
  mover_puntero(20,1);
  int i = 0;
  printf("TSS:",VERDE_L);

  while(i < CANT_TAREAS){
		 printdword(TSS[i].eip,AZUL_L|BASE16);
		 printdword(TSS[i].esp,ROJO_L|BASE16);
		 printdword(TSS[i].ebp,NARANJA_L|BASE16);
		 printdword(i,VERDE_L);
		 printf(",",AZUL_L);
		  i++;
	  }

}
void vaciar_TSS(Tss* tss_task){
	tss_task->cr3 = 0;
}

