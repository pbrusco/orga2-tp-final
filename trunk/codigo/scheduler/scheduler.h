#ifndef __SCHEDULER__
#define __SCHEDULER__

#include "../tipos/tipos_basicos.h"


typedef struct switch_reg_s{
	dword offset;
	word selector;
} __attribute__((__packed__, aligned (8))) switch_reg; 




// TODO: provoca el switch de tareas
void switch_task();

// TODO: funcion que borra todos los datos de una tarea muerta (solo se llama desde la tarea KERNEL)



#endif 
