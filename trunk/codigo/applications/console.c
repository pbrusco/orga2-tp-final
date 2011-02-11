#include "console.h"

char command[100];

void keyCatcher(int key) {
  char c = decode(key);
  if (c == '*') { //if the user press "enter"
   // clear_line();
   // run(command);
  }
  else if (c == '<'){ //if the user press "back"

   // erease_last_char();
   // remove_last_char(command);
  }
  else
  {
    if (true /*exists_place_to_write(command)*/){
      command[0] == c;//add_char(c,command);
      putc(c,0);
   }

  }
}


char decode(int key){
    return 'a';
}


void run (char[] command){
  char[] first_word = extract_first_word(command);
  int second_param = extract_number(command);

  select case (first_word){
    case "help":
      help();
    case "ls":
      show_all();
    case "ps":
      show_running_tasks();
    case "sleeping":
      show_sleeping_tasks();
    case "display":
      display_task(second_param);
    case "merge":
      display_merging_task(second_param);
    case "hide":
      hide_task(second_param);
    case "cls":
      clear_screen();
    case else:
      send_error_message();
    }
}



void help(){}
void show_all(){}
void show_running_tasks(){}
void show_sleeping_tasks(){}
void display_task(int id){}
void display_merging_task(int id){}
void hide_task(int id){}
void clear_screen(){}
void send_error_message(){}

