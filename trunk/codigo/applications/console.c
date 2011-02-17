#include "console.h"
#include "../teclado/teclado.h"

char command_incializado = 'n';
char command[100];
char levanto = 'n';

void console(short int tecla) {

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
    else if (c == '!'){ //if error on pulse


    }

    else
    {
      if (command[0] == '?'){clear_screen();}
      add_char_to_command(c);
      putc(c,VERDE);
    }
  }


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

  help();

  switch(first_word){
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
  printl("HELP: ",0,VERDE,0);
  printl("h: help ",0,AZUL,0);
  printl("l: show_all_tasks: ",0,AZUL,0);
  printl("p: show_running_tasks: ",0,AZUL,0);
  printl("s: show_sleeping_tasks ",0,AZUL,0);
  printl("d: display_task x ",0,AZUL,0);
  printl("m: display_merging_task x",0,AZUL,0);
  printl("i: hide_task x ",0,AZUL,0);

}

void show_all(){
  printf("l: show_all_tasks: ",0,AZUL,0);
}

void show_running_tasks(){
  printf("p: show_running_tasks: ",0,AZUL,0);
}

void show_sleeping_tasks(){
  printf("s: show_sleeping_tasks ",0,AZUL,0);
}

void display_task(int id){
  printf("d: display_task x ",0,AZUL,0);
}

void display_merging_task(int id){
  printf("m: display_merging_task x",0,AZUL,0);
}

void hide_task(int id){
  printf("i: hide_task x ",0,AZUL,0);
}



char extract_code(){
  return 'h';
}

int extract_number(){
  return 1;

}
