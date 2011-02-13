#ifndef __CONSOLE__
#define __CONSOLE__
#include "../pantalla/pantalla.h"
#include "../tipos/tipos_basicos.h"

void console(short int key);
char decode(int key);
void run (command);
void help();
void show_all();
void show_running_tasks();
void show_sleeping_tasks();
void display_task(int id);
void display_merging_task(int id);
void hide_task(int id);
void clear_screen();
void send_error_message();
char extract_first_word();
int extract_number();
#endif

