#include "../tipos/tipos_basicos.h"
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
extern gdt_entry gdt_vector[];

#define TAREAS_EN_MEMORIA 3

Info_Tareas tareas_en_memoria[TAREAS_EN_MEMORIA] = 	{

						(Info_Tareas) {(word) 0, (byte) 0, (byte) 5},
						(Info_Tareas) 	{(word) 0x2000,//eip
								 (byte) 0,//bcp
								 (byte) 0,//gdt
								},
						(Info_Tareas) 	{(word) 0x2040,//eip
								 (byte) 0,//bcp
								 (byte) 0,//gdt
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
	      		run(command);
	      		inicializar_command();
	      		clear_command_line();
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
    case 'c':{
      	clear_screen();
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
	//paso a la pantalla del kernel
	mostrar_pantalla_entera(0);
	clear_screen();

	mover_puntero(3,0);
	printf("HELP: (comandos utiles)\n",VERDE_L);
	printf("h: help(*)\n",AZUL_L);
	printf("l: muestra todas las tareas disponibles(*)\n",AZUL_L);
	printf("p: muestra todas las tareas en ejecucion(*)\n",AZUL_L);
	printf("s: muestra las tareas que no se estan ejecutando(*)\n",AZUL_L);
	printf("d x: mostrar la tarea x\n",AZUL_L);
	printf("z x: inicia la tarea x\n",AZUL_L);
	printf("k x: elimina de la ejecucion a la tarea x\n",AZUL_L);
	printf("c: limpia la pantalla(*)\n",AZUL_L);
	printf("(*) Estos comandos producen un cambio de pantalla a la del kernel", AZUL_L);

}

void cargar_tarea(int id){
 if(id == 0 || id >= TAREAS_EN_MEMORIA ){
		mover_puntero(0,0);
		printf("No existe tal tarea 0 (es el kernel, pero ya esta corriendo)", COLOR_INFO);
	}
	else{
		/*LO SIGUIENTE LO HAGO PARA RELLENAR LOS CAMPOS DE LA INFORMACION DE CADA TAREA,
		Y ASI MANEJARME CON UN UNICO NUMERO DE TAREA A LA HORA DE CARGAR, MATAR Y MOSTRAR*/
		tareas_en_memoria[id].bcp_pos = buscar_entradaBCP_vacia();
		tareas_en_memoria[id].gdt_pos = buscar_entradaGDT_vacia();
		/*********************************************************************************/

		cargarTarea(tareas_en_memoria[id].eip);
		clear_screen();
		printf("Se ha cargado con exito la tarea ", COLOR_INFO);
		printdword(id, COLOR_INFO);
	}
}

void show_all(){
	//paso a la pantalla del kernel
	mostrar_pantalla_entera(0);
	clear_screen();

	mover_puntero(2,0);
	printf("Las tareas disponibles en el sistema son: \n",COLOR_INFO);
	printf("Tarea 1: Relojito rojo \n",COLOR_INFO);
	printf("Tarea 2: Relojito celeste \n",COLOR_INFO);
}

void show_running_tasks(){
	//paso a la pantalla del kernel
	mostrar_pantalla_entera(0);
	clear_screen();

	mover_puntero(2,0);
	printf("Tareas actualmente corriendo(TSS entry en GDT): \n",COLOR_INFO);

	word i = 6;
	while( (gdt_vector[i].atr1 & PRESENTE) == PRESENTE ){
		printdword(i, COLOR_INFO); printf("\n", 0);
		i++;
	}
}

void show_sleeping_tasks(){
	//paso a la pantalla del kernel
	mostrar_pantalla_entera(0);

	printf("s: show_sleeping_tasks ",COLOR_INFO);
}


void display_task(int id){

	clear_screen();

	if (id >= TAREAS_EN_MEMORIA ){
		printf("ERROR!! tarea inexistente",COLOR_INFO);
	}
	else{
		if(BCP[tareas_en_memoria[id].bcp_pos].estado != MUERTO){
			printf("d: display_task ", COLOR_INFO);
			printdword(id,COLOR_INFO);
			mostrar_pantalla_entera(tareas_en_memoria[id].bcp_pos);
		}
		else{
			printf("ERROR: No existe tal tarea", COLOR_INFO);
		}
	}
}

void display_merging_task(int id){
  if (id >= TAREAS_EN_MEMORIA ){
    printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);
  }
  else
  {
    printf("m: display_merging_task ",COLOR_INFO);
    printdword(id,COLOR_INFO);
  }
}

void hide_task(int id){
  if (id >= TAREAS_EN_MEMORIA ){
    printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);
  }
  else
  {
    printf("i: hide_task ",COLOR_INFO);
    printdword(id,COLOR_INFO);
  }
}


void kill_task(int id){
	if(id == 0 || id >= TAREAS_EN_MEMORIA ){
		mover_puntero(0,0);
		printf("No existe tal tarea (0 es el kernel, pero no se mata!)", COLOR_INFO);
	}
	else{
	  clear_screen();
	  printf("k: Mataste a la tarea con id ", COLOR_INFO);
	  printdword(id,COLOR_INFO);
	  matarTarea(tareas_en_memoria[id].bcp_pos);


	  if(tarea_en_pantalla == tareas_en_memoria[id].bcp_pos){
		  //paso a la pantalla del kernel
		  mostrar_pantalla_entera(0);
	  }
	}
}

char extract_code(){
  return command[0];
}

int extract_number(){
  short i;
  int n;
  for(i = 0;i<100;i++){
    if (command[i] == ' ') {
      n = char2num(command[i+1]);
      if (n>0 && n<50){
        return n;
      }
    }
  }
  return 101;

}

