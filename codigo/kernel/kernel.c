#include "kernel.h"
#include "../bcp/bcp.h"
#include "../pantalla/pantalla.h"

extern BCP_Entry BCP[];
extern cursor_consola;

void kernel_infinito(){

	//dejo limpia la pantalla
	clear_screen();

	//inicio el prompt
	clear_command_line();
	
	//inicio el interprete de comandos
	inicializar_command();
	
	
	mover_puntero(10,0);
	printf("LALALA", ROJO_L);

/*	cargarTarea(0x2000);*/
/*	cargarTarea(0x2040);*/


/*	matarTarea(6);*/

	byte aMatar;

	while(true){
	
		// 1ero: busco si hay alguna tarea para matar
		aMatar = buscar_entradaBCP_matar();
		
		//si hay alguna, la hago desaparecer
		if(aMatar != CANT_TAREAS){
			desaparecerTarea(aMatar);
		}

		//2do: hago un cambio de pantalla si fue pedido
		mostrar_pantalla_entera();

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

