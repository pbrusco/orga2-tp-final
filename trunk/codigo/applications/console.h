#ifndef __CONSOLE__
#define __CONSOLE__

void cargar_tarea(int);
void console(short int key);
char decode(int key);
void run (/*command*/);
void add_char_to_command(char c);
void inicializar_command();
void help();
void show_all();
void show_running_tasks();
void show_sleeping_tasks();
void display_task(int id);
void display_merging_task(int id);
void hide_task(int id);
void clear_screen();
void send_error_message();
char extract_code();
int extract_number();
void remove_last_char_from_command();
#endif

