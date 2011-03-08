#include "console.h"
#include "../bcp/bcp.h"
#include "../gdt/gdt.h"
#include "../teclado/teclado.h"
#include "../pantalla/pantalla.h"
#include "../paginacion/paginacion.h"
#include "../memoria/memoria.h"

extern BCP_Entry BCP[];
extern word* cursor_consola, cursor_informacion;
extern byte tarea_a_mostrar, tarea_en_pantalla;

Info_Tareas tareas_en_memoria[3] = 	{	(Info_Tareas) {(word) 0, (byte) 0, (byte) 5},
						(Info_Tareas) 	{(word) 0x2000,//eip
								 (byte) 0,//bcp
								 (byte) 0//gdt
								},
						(Info_Tareas) 	{(word) 0x2040,//eip
								 (byte) 0,//bcp
								 (byte) 0//gdt
								}
					};
					
char command[TAM_COMMAND];
byte command_position = 0;


void console(short int tecla) {
	
	dword cr3_actual;
	dword cr3_kernel = DIR_DIRECTORIO;
	
	//guardo el CR3 de la tarea actualmente ejecutandose
	get_cr3(cr3_actual);
	//seteo como CR3 al del kernel, para que se pueda acceder a toda la memoria
	set_cr3(cr3_kernel);
	

	char c = getChar(tecla & 0x00FF);

	//si el codigo indica que se presiono una tecla
	if(c < 0x80){
	    	if (c == '*') { //if the user press "enter"
	      		clear_command_line();
	      		run(command);
	      		inicializar_command();
	    	}
	    	else if (c == '<'){ //if the user press "back"
	     		remove_last_char_from_command();
			removerc();
		}
	    	else if (c == '!'){//TODO: PARA QUE ES ESTO?????
	    	}
	    	else{
	      		// TODO: esto creo que va a estar demás >> if (command[0] == '?'){clear_screen();}
			add_char_to_command(c);
		      	agregarc(c, COLOR_PROMPT);//TODO: que agregue el caracter a la linea de comando
	    	}
  	}
  	
  	//vuelvo a setear el CR3 como estaba antes de llamar a esta funcion
  	set_cr3(cr3_actual);
}


void inicializar_command(){
	short e;
	for (e = 0; e<TAM_COMMAND; e++){
		command[e] = '?';
	}
	command_position = 0;
}

void remove_last_char_from_command(){
	if(command_position > 0){
		command_position--;
		command[command_position] = '?';
	}
}


void run (){
  char first_word = extract_code(command);
  int second_param = extract_number(command);


  switch(first_word){
    case 'z':
      cargar_tarea(second_param);
      break;
    case 'h':
      help();
      break;
    case 'l':
      show_all();
       break;
    case 'p':
      show_running_tasks();
      break;
    case 's':
      show_sleeping_tasks();
      break;
    case 'd':
      display_task(second_param);
      break;
    case 'm':
      display_merging_task(second_param);
      break;
    case 'i':
      hide_task(second_param);
      break;
    case 'c':{
      	clear_screen();
      	clear_command_line();
      	break;
      }
    case 'k':
    	kill_task(second_param);
    	break;    	
    }

}


void add_char_to_command(char c){
	if(command_position < TAM_COMMAND){
		command[command_position] = c;
		command_position++;
	}
}


void help(){
	mover_puntero(3,0);
	printf("HELP: (comandos utiles)\n",VERDE_L);
	printf("h: help\n",AZUL_L);
	printf("l: show_all_tasks:\n",AZUL_L);
	printf("p: show_running_tasks\n",AZUL_L);
	printf("s: show_sleeping_tasks\n",AZUL_L);
	printf("d: display_task x\n",AZUL_L);
	printf("m: display_merging_task x\n",AZUL_L);
	printf("i: hide_task x\n",AZUL_L);
	printf("z: cargar_tarea x\n",AZUL_L);
	printf("k: matar tarea x\n",AZUL_L);

}

void cargar_tarea(int id){
	if(id == 0){
		mover_puntero(0,0);
		printf("No existe tal tarea 0 (es el kernel, pero ya esta corriendo)", COLOR_INFO);
	}
	else{
		clear_info_line();

		/*LO SIGUIENTE LO HAGO PARA RELLENAR LOS CAMPOS DE LA INFORMACION DE CADA TAREA,
		Y ASI MANEJARME CON UN UNICO NUMERO DE TAREA A LA HORA DE CARGAR, MATAR Y MOSTRAR*/
		tareas_en_memoria[id].bcp_pos = buscar_entradaBCP_vacia();
		tareas_en_memoria[id].gdt_pos = buscar_entradaGDT_vacia();
		/*********************************************************************************/
		
		cargarTarea(tareas_en_memoria[id].eip);
		mover_puntero(0,0);
		printf("Se ha cargado con exito la tarea ", COLOR_INFO);
		printdword(id, COLOR_INFO);
	}
}

void show_all(){
	printf("l: show_all_tasks: ",COLOR_INFO);
}

void show_running_tasks(){
  printf("p: show_running_tasks: ",COLOR_INFO);
}

void show_sleeping_tasks(){
  printf("s: show_sleeping_tasks ",COLOR_INFO);
}


void display_task(int id){
	
	clear_info_line();
	mover_puntero(0,0);
	
	if (id == -1){
		printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);
	}
	else{
		if(BCP[id].estado != MUERTO){
			printf("d: display_task ", COLOR_INFO);
			printdword(id,COLOR_INFO);
			cambiar_de_pantalla(tareas_en_memoria[id].bcp_pos);
		}
		else{
			printf("ERROR: No existe tal tarea", COLOR_INFO);
		}
	}
}

void display_merging_task(int id){
  if (id == -1){
    printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);
  }
  else
  {
    printf("m: display_merging_task ",COLOR_INFO);
    printdword(id,COLOR_INFO);
  }
}

void hide_task(int id){
  if (id == -1){
    printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);
  }
  else
  {
    printf("i: hide_task ",COLOR_INFO);
    printdword(id,COLOR_INFO);
  }
}


void kill_task(int id){
	clear_info_line();
	mover_puntero(0,0);
	printf("k: Mataste a la tarea con id ", COLOR_INFO);
	printdword(id,COLOR_INFO);
	matarTarea(tareas_en_memoria[id].gdt_pos);
	//TODO: por ahora queda así, pero se puede poner aca que si se mata a la tarea en pantalla, se pasa a mostrar la pantalla del kernel...
}

char extract_code(){
  return command[0];
}

int extract_number(){
  short i;
  for(i = 0;i<100;i++){
    if (command[i] == ' ') {
      return char2num(command[i+1]);
    }
  }
  return -1;

}

