#include "../tipos/tipos_basicos.h"
#include "console.h"
#include "../bcp/bcp.h"
#include "../gdt/gdt.h"
#include "../teclado/teclado.h"
#include "../pantalla/pantalla.h"
#include "../paginacion/paginacion.h"
#include "../memoria/memoria.h"

extern BCP_Entry BCP[];
extern uint16* cursor_consola, cursor_informacion;
extern uint8 tarea_a_mostrar, tarea_en_pantalla;
extern gdt_entry gdt_vector[];

#define TAREAS_EN_MEMORIA 3

Info_Tareas tareas_en_memoria[TAREAS_EN_MEMORIA] = 	{

						(Info_Tareas) {(uint32) 0, (uint32) 0, "kernel"},
						(Info_Tareas) 	{
								(uint32) 0x2000,//eip
								(uint32) 512,//tam
								(int8*) "relojito_rojo",//nombre
								},
						(Info_Tareas) 	{
								(uint32) 0x2000 + 512,//eip
								(uint32) 512,//tam
								(int8*) "relojito_celeste",//nombre
								}
							};

int8 command[TAM_COMMAND];
uint8 command_position = 0;


void console(int16 tecla) {

	uint32 cr3_actual;
	uint32 cr3_kernel = DIR_DIRECTORIO;

	//guardo el CR3 de la tarea actualmente ejecutandose
	get_cr3(cr3_actual);
	//seteo como CR3 al del kernel, para que se pueda acceder a toda la memoria
	set_cr3(cr3_kernel);


	int8 c = getChar(tecla & 0x00FF);

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
	int16 e;
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
  int8 first_word = extract_code(command);
  int32 second_param = extract_number(command);


  switch(first_word){
    case 'z':
      cargar_tarea(second_param);
      break;
    case 'v':
      cargar_tarea_y_mostrar(second_param);
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


void add_char_to_command(int8 c){
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
	printf("v x: carga y muestra la tarea x\n",AZUL_L);
	printf("d x: mostrar la tarea x\n",AZUL_L);
	printf("z x: inicia la tarea x\n",AZUL_L);
	printf("k x: elimina de la ejecucion a la tarea x\n",AZUL_L);
	printf("c: limpia la pantalla(*)\n",AZUL_L);
	printf("(*) Estos comandos producen un cambio de pantalla a la del kernel", AZUL_L);

}

void cargar_tarea(int32 id){
	clear_info_line();
	mover_puntero(0,0);

 	if(id == 0 || id >= TAREAS_EN_MEMORIA ){

		printf("No existe tal tarea 0 (es el kernel, pero ya esta corriendo)", COLOR_INFO);
	}
	else{
		uint16 pid = cargarTarea(tareas_en_memoria[id].eip, tareas_en_memoria[id].tam, tareas_en_memoria[id].nombre);
   	 	printf("Se ha cargado con exito la tarea, PID: ", COLOR_INFO);
		printdword(pid, COLOR_INFO);
	}
}

void cargar_tarea_y_mostrar(int32 id){
 if(id == 0 || id >= TAREAS_EN_MEMORIA ){
		mover_puntero(0,0);
		printf("No existe tal tarea 0 (es el kernel, pero ya esta corriendo)", COLOR_INFO);
	}
	else{
		uint16 pid = cargarTarea(tareas_en_memoria[id].eip, tareas_en_memoria[id].tam, tareas_en_memoria[id].nombre);
    	display_task(pid);

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
	printf("\tNombre\t|\tPID\n",COLOR_INFO);

	uint16 i = 6;
	uint8 bcpPos;
	while( i < (6+CANT_TAREAS) ){
		if( (gdt_vector[i].atr1 & PRESENTE) == PRESENTE ){
			bcpPos = buscar_entradaBCP(i);
			printf(BCP[bcpPos].nombre, COLOR_INFO);
			printf("	|	", COLOR_INFO);
			printdword(i, COLOR_INFO); printf("\n", 0);
		}
		i++;
	}
}

void display_task(int32 id){
	//recordar que "id" es el indice en la gdt del segmento tss de una tarea

	clear_screen();

	//si no está presente la tarea
	if ( (gdt_vector[id].atr1 & PRESENTE) != PRESENTE ){
		printf("ERROR!! tarea inexistente",COLOR_INFO);
	}
	else{
		uint8 bcpPos = buscar_entradaBCP(id);
		if(BCP[bcpPos].estado != MUERTO){
			printf("mostrando tarea ", COLOR_INFO);
			printdword(id,COLOR_INFO);
			mostrar_pantalla_entera(bcpPos);
		}
		else{
			printf("ERROR: No existe tal tarea", COLOR_INFO);//TODO: esto creo que esta demás
		}
	}
}

void display_merging_task(int32 id){
  if (id >= TAREAS_EN_MEMORIA ){
    printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);
  }
  else
  {
    printf("m: display_merging_task ",COLOR_INFO);
    printdword(id,COLOR_INFO);
  }
}

void hide_task(int32 id){
  if (id >= TAREAS_EN_MEMORIA ){
    printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);
  }
  else
  {
    printf("i: hide_task ",COLOR_INFO);
    printdword(id,COLOR_INFO);
  }
}


void kill_task(int32 id){//recordar que id es el indice en la gdt del segmento tss de una tarea
	mover_puntero(0,0);
	if( (gdt_vector[id].atr1 & PRESENTE) != PRESENTE ){
		printf("No existe tal tarea (0 es el kernel, pero no se mata!)", COLOR_INFO);
	}
	else{
	  clear_screen();
	  printf("k: Mataste a la tarea con id ", COLOR_INFO);
	  printdword(id,COLOR_INFO);
	  matarTarea(id);//recordar que "id" es la posicion en la GDT del segmento tss de una tarea

	}
}

//TODO: estas funciones, no podrian hacerse mejor? digo, que levanten cualquier codigo y cualquier numero

int8 extract_code(){
  return command[0];
}

int32 extract_number(){
  int16 i;
  int32 n;
  for(i = 0;i<100;i++){
    if (command[i] == ' ') {
      n = char2num(command[i+1]);
      if (n>0 && n<50){
        return n;
      }
    }
  }
  return 101;//TODO: POR QUE?

}

