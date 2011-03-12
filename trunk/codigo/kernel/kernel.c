#include "../tipos/tipos_basicos.h"
#include "kernel.h"
#include "../bcp/bcp.h"
#include "../pantalla/pantalla.h"
#include "../applications/console.h"
#include "../scheduler/scheduler.h"

extern BCP_Entry BCP[];
extern uint16* cursor_consola;

void kernel_infinito(){

	//dejo limpia la pantalla
	clear_screen();

	//inicio el prompt
	clear_command_line();
	
	//inicio el interprete de comandos
	inicializar_command();

	uint8 aMatar;

	while(TRUE){
	
		// 1ero: busco si hay alguna tarea para matar
		aMatar = buscar_entradaBCP_matar();


		//si hay alguna, la hago desaparecer
		if(aMatar != CANT_TAREAS){
			desaparecerTarea(aMatar);
			//TODO: ver si se puede hacer algo con "kill_app", que tira un error "codigo de operacion invalido"
			//cuando se la llama desde console (creo que cuando se la llama desde el contexto de la tarea)
		}
	}
}



// esto lo puse por si a alguno le interesa
void update_cursor(int32 row, int32 col){
	uint16 position=(row*80) + col;

	// cursor LOW port to vga INDEX register
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8)(position&0xFF));
	// cursor HIGH port to vga INDEX register
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8)((position>>8)&0xFF));
}

