#ifndef __CONSOLE__
#define __CONSOLE__

#define TAM_COMMAND 80

void cargar_tarea(int32); //recibe la posicion de la tarea a cargar dentro de "tareas_en_memoria"

void console(int16 key);
int8 decode(int32 key);
void run (/*command*/);
void add_char_to_command(int8 c);
void inicializar_command();
void help();
void show_all();
void show_running_tasks();
void show_sleeping_tasks();

void display_task(int32 id); //recibe la posicion de la tarea a mostrar dentro de "tareas_en_memoria"
void cargar_tarea_y_mostrar(int32 id);
void display_merging_task(int32 id);
void hide_task(int32 id);
void clear_screen();
void send_error_message();
int8 extract_code();
int32 extract_number();
void remove_last_char_from_command();

void kill_task(int32 id); //recibe la posicion de la tarea a matar dentro de "tareas_en_memoria"



typedef struct Info_Tareas_s{
	uint32 eip;
	uint32 tam;
	int8* nombre;
} __attribute__((__packed__, aligned (8))) Info_Tareas;


#endif

