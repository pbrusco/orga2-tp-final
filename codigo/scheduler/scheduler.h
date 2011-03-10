#ifndef __SCHEDULER__
#define __SCHEDULER__


typedef struct switch_reg_s{
	dword offset;
	word selector;
} __attribute__((__packed__, aligned (8))) switch_reg; 




// provoca el switch de tareas
void switch_task();



#endif 
