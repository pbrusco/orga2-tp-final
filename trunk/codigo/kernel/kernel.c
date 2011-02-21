#include "kernel.h"
#include "../bcp/bcp.h"
#include "../pantalla/pantalla.h"

extern BCP_Entry BCP[];

#define outb(p,v) __asm__ __volatile__("outb %%al, %%dx" : : "al"(v), "dx"(p) )

void kernel_infinito(){
		
/*	cargarTarea(0x2000);*/
/*	cargarTarea(0x2040);*/


/*	int contador = 10000;*/
/*	while(contador != 0){contador--;}*/

/*	matarTarea(6);*/

	byte aMatar;
	
	while(true){
		aMatar = buscar_entradaBCP_matar();
		if(aMatar != CANT_TAREAS){
			desaparecerTarea(aMatar);
		}
	}
}



// esto lo puse por si a alguno le interesa
void update_cursor(int row, int col){
	unsigned short position=(row*80) + col;
 
	// cursor LOW port to vga INDEX register
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char)(position&0xFF));
	// cursor HIGH port to vga INDEX register
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}
