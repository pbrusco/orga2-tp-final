#include "scheduler.h"
#include "../bcp/bcp.h"

// este registro lo voy a usar para hacer el "jmp" para cambiar de tarea
switch_reg salto;

dword* switch_task(){

	//1ro: me fijo que haya mas de una tarea, sino termino
	
	//2do: cambio el estado de la tarea actual de CORRIENDO a ACTIVO, paso la tarea actual
	//a la siguiente y cambio el estado de la tarea siguiente de ACTIVO a CORRIENDO
	
	//3ro: devolver el puntero a donde se debe hacer el jmp
	// TODO: hacer el jmp aca con assembler inline
}
