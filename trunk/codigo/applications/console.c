#include "console.h"
#include "../bcp/bcp.h"
#include "../teclado/teclado.h"
#include "../pantalla/pantalla.h"
#include "../paginacion/paginacion.h"
#include "../memoria/memoria.h"

extern BCP_Entry BCP[];
extern word* cursor_consola, cursor_informacion;
extern byte tarea_a_mostrar, tarea_en_pantalla;

dword posicion_de_las_tareas_en_memoria[2] = {0x2000,0x2040};
char command[TAM_COMMAND];
byte command_position = 0;


void console(short int tecla) {
	/*RECORDAR QUE CUANDO SE INGRESA ACA, NO IMPORTA QUE TAREA SE ESTE EJECUTANDO, SIEMPRE SE INGRESA UTILIZANDO EL DIRECTORIO DEL KERNEL*/
	/*esto es para que siempre se escriba en la pantalla*/
  		//1ero: guardo el mapeo de la pantalla
  		dword entrada_video = obtener_mapeo(BCP[0].entrada_directorio, DIR_INI_PANTALLA);
  		//2do: mapeo la entrada de video a la pantalla
  		mapear_pagina(	BCP[0].entrada_directorio, 
  				DIR_INI_PANTALLA, 
  				DIR_INI_PANTALLA,
  				(word) (entrada_video & 0x00000FFF));
  		
  	/**/

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

	/*dejo el mapeo de video igual que estaba antes*/
	mapear_pagina(BCP[0].entrada_directorio, DIR_INI_PANTALLA, entrada_video, (word) (entrada_video & 0x00000FFF));

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
    case 'c':
      clear_screen();
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

}

void cargar_tarea(int id){
	clear_info_line();
	cargarTarea(posicion_de_las_tareas_en_memoria[id]);
	mover_puntero(0,0);
	printf("Se ha cargado con exito la tarea ", COLOR_INFO);
	printdword(id, COLOR_INFO);
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
	//TODO: ver por que no funciona el genérico!!!
/*	if (id == -1){*/
/*		printf("ERROR!! Fijate el parametro vistes",COLOR_INFO);*/
/*	}*/
/*	else{*/
/*		clear_info_line();*/
/*		mover_puntero(0,0);*/
/*		printf("d: display_task ", COLOR_INFO);*/
/*		printdword(id,COLOR_INFO);*/
/*		cambiar_de_pantalla(id);*/
/*	}*/

	if(id == 1){
		//copio la pantalla a donde debe escribir realmente el kernel
		cpmem((byte*) ( DIR_INI_PANTALLA + (80*2) ),
		 		(byte*) ( ((dword) BCP[0].pantalla) + (80*2)),
		 		TAM_PANTALLA_TAREA);

		//copio la pagina de video de la "tarea_a_mostrar" a la pantalla
		cpmem(	(byte*) (((dword) BCP[1].pantalla) + 80*2),
			(byte*) (DIR_INI_PANTALLA + (80*2)),
			TAM_PANTALLA_TAREA);
			
		//remapeo la pagina de video de la tarea a donde le corresponde escribir
		mapear_pagina(	BCP[0].entrada_directorio,
				DIR_INI_PANTALLA,
				(dword) BCP[0].pantalla,
				USUARIO | WRITE | PRESENT);

		//remapeo la pagina de video de la "tarea_a_mostrar" a la pantalla
		mapear_pagina(	BCP[1].entrada_directorio,
				DIR_INI_PANTALLA,
				DIR_INI_PANTALLA,
				USUARIO | WRITE | PRESENT);
	}
	if(id == 0){
		//copio la pantalla a donde debe escribir realmente la tarea_en_pantalla
		cpmem((byte*) ( DIR_INI_PANTALLA + (80*2) ),
		 		(byte*) ( ((dword) BCP[1].pantalla) + (80*2)),
		 		TAM_PANTALLA_TAREA);

		//copio la pagina de video de la "tarea_a_mostrar" a la pantalla
		cpmem(	(byte*) (((dword) BCP[0].pantalla) + 80*2),
			(byte*) (DIR_INI_PANTALLA + (80*2)),
			TAM_PANTALLA_TAREA);
		
		//remapeo la pagina de video de la tarea a donde le corresponde escribir
		mapear_pagina(	BCP[1].entrada_directorio,
				DIR_INI_PANTALLA,
				(dword) BCP[1].pantalla,
				USUARIO | WRITE | PRESENT);

		//remapeo la pagina de video de la "tarea_a_mostrar" a la pantalla
		mapear_pagina(	BCP[0].entrada_directorio,
				DIR_INI_PANTALLA,
				DIR_INI_PANTALLA,
				USUARIO | WRITE | PRESENT);
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

