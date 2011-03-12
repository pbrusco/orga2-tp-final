#ifndef __SCHEDULER__
#define __SCHEDULER__


typedef struct switch_reg_s{
	uint32 offset;
	uint16 selector;
} __attribute__((__packed__, aligned (8))) switch_reg;




// provoca el switch de tareas
void switch_task();
void info_SCHEDULER();


#endif

