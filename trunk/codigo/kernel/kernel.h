#ifndef __KERNEL__
#define __KERNEL__

#include "../tipos/tipos_basicos.h"
 
/*ESTAS DEFINICIONES SIRVEN PARA HACER SALIDA DE VALORES A PUERTOS*/
#define outb(p,v) __asm__ __volatile__("outb %%al, %%dx" : : "al"(v), "dx"(p) )
#define outw(p,v) __asm__ __volatile__("outw %%al, %%dx" : : "ax"(v), "dx"(p) )
#define outd(p,v) __asm__ __volatile__("outd %%al, %%dx" : : "eax"(v), "dx"(p) )



/* TODO: Esta función debería hacer todo lo que se hace en "kernel.asm" desde que se mueve la pila hasta que se llega al "jmp $", reemplazando este ultimo por un llamado a la funcion "kernel_infinito" */
void kernel_init();

/*Esta función va a ejecutarse por siempre por el kernel. Todo lo que necesita ser hecho por el kernel (con su paginacion y sus privilegios) debe ir aqui, con aquellas funciones criticas puestas entre un "cli" y un "sti" para no ser interrumpido. 
1) Si hay alguna BCP marcada como "MATAR", hay que hacerla desaparecer (parezco un militar con esto :P)
*/
void kernel_infinito();


//esta funcion mueve el cursor que titila en la pantalla
void update_cursor(int row, int col);

#endif 
