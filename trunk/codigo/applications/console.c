#include "console.h"

char command[100];

void console(int key) {
  //command = "help";


  char c = decode(key);
  if (c == '*') { //if the user press "enter"
   // clear_line();
    run(command);
  }
  else if (c == '<'){ //if the user press "back"

   // erease_last_char();
   // remove_last_char(command);
  }
  else
  {
    //if (1){ /*exists_place_to_write(command)*/
      //command[0] == c;//add_char(c,command);
   putc(c,0);
   }

  }



char decode(int key){
    return '*';
}


void run (){
  char first_word = extract_first_word(command);
  int second_param = extract_number(command);
  clear_screen();
  printf("lalalala", 0, 0, 0);

/*
  switch(first_word){
    case "help":
      help();
      break;
    case "ls":
      show_all();
       break;
    case "ps":
      show_running_tasks();
      break;
    case "sleeping":
      show_sleeping_tasks();
      break;
    case "display":
      display_task(second_param);
      break;
    case "merge":
      display_merging_task(second_param);
      break;
    case "hide":
      hide_task(second_param);
      break;
    case "cls":
      clear_screen();
      break;
    case else:
      send_error_message();
      break;
    }
    */
}



void help(){
  putc('c',0);
}

void show_all(){
  putc('c',0);
}

void show_running_tasks(){
  putc('c',0);
}

void show_sleeping_tasks(){
  putc('c',0);
}

void display_task(int id){
  putc('c',0);
}

void display_merging_task(int id){
  putc('c',0);
}

void hide_task(int id){
  putc('c',0);
}

void send_error_message(){
  putc('c',0);
}

char extract_first_word(){
  return 'a';
}

int extract_number(){
  return 1;

}

