#include "scheduler.h"
#include "../bcp/bcp.h"
#include "../pantalla/pantalla.h"

extern BCP_Entry BCP[];

// este registro lo voy a usar para hacer el "jmp" para cambiar de tarea
switch_reg salto;


// devuelve 1 si hubo cambio de tarea, 0 si no
void switch_task(){
	
	//1ro: me fijo que haya mas de una tarea, sino termino
	if(cant_tareas_en_sistema != 1){

		//2do: cambio el estado de la tarea actual de CORRIENDO a ACTIVO, paso la tarea actual
		//a la siguiente y cambio el estado de la tarea siguiente de ACTIVO a CORRIENDO
		BCP[tarea_actual].estado = ACTIVO;
		tarea_actual = BCP[tarea_actual].sig;
		BCP[tarea_actual].estado = CORRIENDO;		
		
		//3ro: cambio el selector de "salto" a donde marque el "pid" de la tarea_actual
		salto.selector = BCP[tarea_actual].pid * 8;//multiplicopor 8 porque cada entrada es de 8 bytes
		
		//4to: hago el cambio de tarea
		//__asm__ __volatile__ ( "xchg %bx, %bx");	
		__asm__ __volatile__ ( "ljmp (salto)");	
	}
}
