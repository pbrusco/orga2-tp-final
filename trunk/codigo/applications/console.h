#ifndef __CONSOLE__
#define __CONSOLE__

#define TAM_COMMAND 80

void cargar_tarea(int); //recibe la posicion de la tarea a cargar dentro de "tareas_en_memoria"

void console(short int key);
char decode(int key);
void run (/*command*/);
void add_char_to_command(char c);
void inicializar_command();
void help();
void show_all();
void show_running_tasks();
void show_sleeping_tasks();

void display_task(int id); //recibe la posicion de la tarea a mostrar dentro de "tareas_en_memoria"
void cargar_tarea_y_mostrar(int id);
void display_merging_task(int id);
void hide_task(int id);
void clear_screen();
void send_error_message();
char extract_code();
int extract_number();
void remove_last_char_from_command();

void kill_task(int id); //recibe la posicion de la tarea a matar dentro de "tareas_en_memoria"



typedef struct Info_Tareas_s{
	word eip;
	byte bcp_pos;
	byte gdt_pos;
} __attribute__((__packed__, aligned (8))) Info_Tareas;


#endif

