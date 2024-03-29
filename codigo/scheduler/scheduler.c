#include "../tipos/tipos_basicos.h"
#include "scheduler.h"
#include "../bcp/bcp.h"
#include "../pantalla/pantalla.h"

extern BCP_Entry BCP[];
extern uint8 tarea_actual;

// este registro lo voy a usar para hacer el "jmp" para cambiar de tarea
switch_reg salto;


void info_SCHEDULER(){

  mover_puntero(10,10);
  int x = 1;
  info_BCP(0);
  if (BCP[0].sig != 0){
    while(x != 0){
     info_BCP(x);
      x = BCP[x].sig;
    }
  }



}
void switch_task(){
//recordar que las interrupciones en el contexto de esta tarea estan deshabilitadas

  //info_SCHEDULER();
  //info_GDT();
  //info_TSS();

	//1ro: me fijo que haya mas de una tarea, sino termino
	if(tarea_actual != BCP[tarea_actual].sig){

		//2do: cambio el estado de la tarea actual de CORRIENDO a ACTIVO
		//(o lo dejo en MATAR si es que estaba asi), paso la tarea actual
		//a la siguiente y cambio el estado de la tarea siguiente de ACTIVO a CORRIENDO
		if(BCP[tarea_actual].estado != MATAR){
			BCP[tarea_actual].estado = ACTIVO;
		}
		tarea_actual = BCP[tarea_actual].sig;
		BCP[tarea_actual].estado = CORRIENDO;

		//3ro: cambio el selector de "salto" a donde marque el "pid" de la tarea_actual
		//teniendo en cuenta de qué RPL elegir dependiendo de qué tarea se va a pasar a ejecutar
		if(tarea_actual != 0){
			salto.selector = BCP[tarea_actual].pid * 8 | 3;//multiplicopor 8 porque cada entrada es de 8 bytes
		}
		else{
		  salto.selector = BCP[tarea_actual].pid * 8;
		}


		//4to: hago el cambio de tarea
		__asm__ __volatile__ ( "ljmp *(salto)");
	}
}

