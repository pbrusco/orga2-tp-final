#include "console.h"
#include "../bcp/bcp.h"
#include "../teclado/teclado.h"
#include "../pantalla/pantalla.h"
#include "../paginacion/paginacion.h"
#include "../memoria/memoria.h"
extern BCP_Entry BCP[];

dword posicion_de_las_tareas_en_memoria[2] = {0x2000,0x2040};
char command_incializado = 'n';
char command[100];
char levanto = 'n';

void console(short int tecla) {
	/*esto es para que siempre se escriba en la pantalla*/
  		//1ero: guardo el mapeo de la pantalla
  		dword entrada_video = obtener_mapeo(BCP[tarea_actual].entrada_directorio, DIR_INI_PANTALLA);
  		//2do: mapeo la entrada de video a la pantalla
  		mapear_pagina(BCP[tarea_actual].entrada_directorio, DIR_INI_PANTALLA, DIR_INI_PANTALLA, WRITE | PRESENT | USUARIO);
  		
  	/**/

  if (command_incializado == 'n'){
    inicializar_command();
  }

  char c = getChar(tecla & 0x00FF);

  if (levanto=='s'){levanto = 'n';}
  else{
    levanto = 's';
    if (c == '*') { //if the user press "enter"
      clear_screen(); //TODO: replace by clear_line();
      run(command);
      inicializar_command();

    }
    else if (c == '<'){ //if the user press "back"
     remove_last_char_from_command();
     borrarc();
    }
    else if (c == '!'){}
    else
    {
      if (command[0] == '?'){clear_screen();}
      add_char_to_command(c);
      putc(c,VERDE_L);

    }
  }

	/*dejo el mapeo de video igual que estaba antes*/
	mapear_pagina(BCP[tarea_actual].entrada_directorio, DIR_INI_PANTALLA, entrada_video, (word) (entrada_video & 0x00000FFF));

}


void inicializar_command(){
  short e;
    for (e = 0; e<100; e++){
       command[e] = '?';
    }
    command_incializado = 'y';
}

void remove_last_char_from_command(){

  short i;
  for (i = 99; i>=0;i--){
    if (command[i] != '?'){
      command[i] = '?';
      i = -1;
    }
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

  short i;
  for (i = 0; i<100;i++){
    if (command[i] == '?'){
      command[i] = c;
      i = 100;
    }
  }
}


void help(){
  printf("HELP: (comandos utiles) ",VERDE_L);
  printl("h: help ",0,AZUL_L,0);
  printl("l: show_all_tasks: ",0,AZUL_L,0);
  printl("p: show_running_tasks: ",0,AZUL_L,0);
  printl("s: show_sleeping_tasks ",0,AZUL_L,0);
  printl("d: display_task x ",0,AZUL_L,0);
  printl("m: display_merging_task x",0,AZUL_L,0);
  printl("i: hide_task x ",0,AZUL_L,0);
  printl("z: cargar_tarea x ",0,AZUL_L,0);

}

void cargar_tarea(int id){
  cargarTarea(posicion_de_las_tareas_en_memoria[id]);
}

void show_all(){
  printf("l: show_all_tasks: ",AZUL_L);
}

void show_running_tasks(){
  printf("p: show_running_tasks: ",AZUL_L);
}

void show_sleeping_tasks(){
  printf("s: show_sleeping_tasks ",AZUL_L);
}

void display_task(int id){
  if (id == -1){
    printf("ERROR!! Fijate el parametro vistes",AZUL_L);
  }
  else
  {
    printf("d: display_task ",AZUL_L);
    putc(num2char2(id),AZUL_L);
  }
}

void display_merging_task(int id){
  if (id == -1){
    printf("ERROR!! Fijate el parametro vistes",AZUL_L);
  }
  else
  {
    printf("m: display_merging_task ",AZUL_L);
    putc(num2char2(id),AZUL_L);
  }
}

void hide_task(int id){
  if (id == -1){
    printf("ERROR!! Fijate el parametro vistes",AZUL_L);
  }
  else
  {
    printf("i: hide_task ",AZUL_L);
    putc(num2char2(id),AZUL_L);
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

