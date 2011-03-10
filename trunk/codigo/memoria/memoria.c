#include "../tipos/tipos_basicos.h"
#include "memoria.h"

void contarMemoria(){
	//arranco suponiendo que el sistema cuenta con al menos 2 mb
	dword *dir = (dword *) (0x2*MB);
	dword dato = 0x12345678;

	*dir = dato;
	
	while(( (dword) dir < (32*MB)) &&  *dir == dato){
		*dir = 0;
		dir += 1024;
		*dir = dato;
	}
	//no se le suma 2 MB porque la direccion ya apunta al total de la memoria (en bytes)
	memoria_total = ((dword) dir) / (1*MB);
	
	paginas_libres = ( (memoria_total * MB) - (2*MB) ) / TAM_PAG ;
}


//Arma el bitmap al inicio. Marca como ocupados los lugares reservados para el kernel
void llenarBitmap(){

	//calculo cuantas paginas ocupa el directorio y las tablas de paginas del kernel
	unsigned long long offset_tablas = (memoria_total / 4) + 1;

	//calculo donde arranca el bitmap, justo debajo de las tablas de paginas
	dir_init_bitmap = (byte *) DIR_DIRECTORIO + (offset_tablas * TAM_PAG);
	
	//calculo donde termina el bitmap (ultima dir valida del bitmap)
	dir_end_bitmap =  (byte *)   (((dword) dir_init_bitmap) + ((((memoria_total * MB) / TAM_PAG)/8) - 1));

	int pag_ocupadas = ((DIR_KERNEL_END) / TAM_PAG)/8;
	
	byte* bitmap = dir_init_bitmap;
	
	
	while(0 < pag_ocupadas)
	{
		*bitmap = 0xFF;
		bitmap++;
		pag_ocupadas--;
	}

	while(bitmap <= dir_end_bitmap)
	{
		*bitmap = 0x00;
		bitmap++;
	}
}

//Devuelve un puntero a una pagina vacia, y la marca como ocupada en el bitmap
dword* pidoPagina() {

	byte* bitmap_dir = dir_init_bitmap + (((2*MB)/TAM_PAG)/8);//comienzo del bitmap + offset ocupado
	byte libre;
	dword contador_paginas = 512;//desde los 2 MB en adelante
	dword var = 0;

	while(bitmap_dir <= dir_end_bitmap && *bitmap_dir == 0xFF){
		bitmap_dir++;
		contador_paginas = contador_paginas + 8;
	}

	if(bitmap_dir <= dir_end_bitmap) {

		libre = *bitmap_dir;
		 
		int i;
		for(i = 0;i<8;i++) {
			if ((libre & 1) == 0) {
				var = contador_paginas + i;
				*bitmap_dir = *bitmap_dir | (1 << i); //Se actualiza el BIT correspondiente a la pagina obtenida
				
				//actualizo la cant de paginas libres
				paginas_libres--;
				
				return (dword *) (var*4*1024);
			}
			libre = libre >> 1;
		}
	}
	
	return 0;
}


//Dada una dir de una pagina, la libera dentro del bitmap
void liberoPagina(dword* pagina){
	
	if( ((dword) pagina) >= 2*MB){
		//me guardo el numero de la pagina que se quiere liberar, comenzando a contar desde los 2MB
		dword numero_de_pagina = (((dword) pagina) / TAM_PAG) - ( (dword) (2*MB / TAM_PAG) );

		//comienzo del bitmap + offset ocupado
		byte* bitmap_dir = dir_init_bitmap + (((2*MB)/TAM_PAG)/8);

		//muevo el puntero a donde corresponde y me fijo que pagina dentro de ese puntero debo liberar
		bitmap_dir += numero_de_pagina/8;
		numero_de_pagina = numero_de_pagina - (8*(numero_de_pagina/8));	

		byte mascara = 0x01;
		mascara = mascara << numero_de_pagina;

		//actualizo el valor del bitmap, liberando la pagina
		*bitmap_dir = *bitmap_dir & (mascara ^ 0xFF); // el operador ^ es el XOR, con FF invierto todos los bits
	
		//actualizo la cant de paginas libres
		paginas_libres++;
	}
}


void setmem(byte* dir, byte set, dword cant){
	while(cant > 0){
		*dir = set;
		dir++;
		cant--;
	}
}


void cpmem(byte* from, byte* to, dword cant){
	while(cant > 0){
		*to = *from;
		to++; from++;
		cant--;
	}
}







/***********************************************************************/
/*********ESTO ES PARA VER A DONDE ESTA UBICADO EL KERNEL ENTERO********/
/***********************************************************************/
#include "../bcp/bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"
#include "../interrupciones/idt.h"
#include "../interrupciones/isr.h"
#include "../paginacion/paginacion.h"
#include "../scheduler/scheduler.h"
#include "../pantalla/pantalla.h"
#include "../teclado/teclado.h"
#include "../applications/console.h"
#include "../kernel/kernel.h"

extern Info_Tareas tareas_en_memoria[];
extern char command[];


extern IDT_Descriptor IDT_DESC;
extern IDT_Entry IDT[];

extern word* puntero_pantalla;
extern byte tarea_en_pantalla, tarea_a_mostrar;

extern switch_reg salto;
extern char teclado[];
extern gdt_entry gdt_vector[];


void donde_esta_el_kernel(){
	
	clear_screen();
	//CONSOLE
	printf("CONSOLA", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("consoleD: ",VERDE_L | BRILLANTE); printdword((dword) &console, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("extraerNumero: ",VERDE_L | BRILLANTE); printdword((dword) &extract_number, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("tareas_en_memoria_info(var): ",VERDE_L | BRILLANTE); printdword((dword) &tareas_en_memoria, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("command[0]: ",VERDE_L | BRILLANTE); printdword((dword) &command, BASE16 | VERDE_L | BRILLANTE);
	printf("      command[99]: ",VERDE_L | BRILLANTE); printdword((dword) &command[99], BASE16 | VERDE_L | BRILLANTE);

	//BCP
	salto_de_linea();salto_de_linea();
	printf("BCP", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("tarea_actual: ",VERDE_L | BRILLANTE); printdword((dword) &tarea_actual, BASE16 | VERDE_L | BRILLANTE);	
	salto_de_linea();
	printf("cant_tareas_en_sistema: ",VERDE_L | BRILLANTE); printdword((dword) &cant_tareas_en_sistema, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("iniciar_BCP: ",VERDE_L | BRILLANTE); printdword((dword) &iniciar_BCP, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("desaparecerTarea: ",VERDE_L | BRILLANTE); printdword((dword) &desaparecerTarea, BASE16 | VERDE_L | BRILLANTE);
	
	
	//GDT
	salto_de_linea();salto_de_linea();
	printf("GDT", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("gdt_vector[0]: ",VERDE_L | BRILLANTE); printdword((dword) &gdt_vector[0], BASE16 | VERDE_L | BRILLANTE);
	printf("      gdt_vector[127]: ",VERDE_L | BRILLANTE); printdword((dword) &gdt_vector[127], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("make_descriptor: ",VERDE_L | BRILLANTE); printdword((dword) &make_descriptor, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("borrar_gdt_entry: ",VERDE_L | BRILLANTE); printdword((dword) &borrar_gdt_entry, BASE16 | VERDE_L | BRILLANTE);
	
	//INTERRUPCIONES
	salto_de_linea();salto_de_linea();
	printf("INTERRUPCIONES", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("idt_fill: ",VERDE_L | BRILLANTE); printdword((dword) &idtFill, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("idt[0]: ",VERDE_L | BRILLANTE); printdword((dword) &IDT[0], BASE16 | VERDE_L | BRILLANTE);
	printf("      idt[255]: ",VERDE_L | BRILLANTE); printdword((dword) &IDT[255], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("_INT0: ",VERDE_L | BRILLANTE); printdword((dword) &_INT0 , BASE16 | VERDE_L | BRILLANTE);
	printf("      _INT80: ",VERDE_L | BRILLANTE); printdword((dword) &_INT80 , BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("IDT_DESC: ",VERDE_L | BRILLANTE); printdword((dword) &IDT_DESC, BASE16 | VERDE_L | BRILLANTE);
	
	
	breakpoint();
	clear_screen();
	
	
	//KERNEL
	printf("KERNEL", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("kernel_infinito: ",VERDE_L | BRILLANTE); printdword((dword) &kernel_infinito, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("update_cursor: ",VERDE_L | BRILLANTE); printdword((dword) &update_cursor, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//MEMORIA
	salto_de_linea();salto_de_linea();
	printf("MEMORIA", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("memoria_total: ",VERDE_L | BRILLANTE); printdword((dword) &memoria_total, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("&dir_init_bitmap: ",VERDE_L | BRILLANTE); printdword((dword) &dir_init_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("&dir_end_bitmap: ",VERDE_L | BRILLANTE); printdword((dword) &dir_end_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("paginas_libres: ",VERDE_L | BRILLANTE); printdword((dword) &paginas_libres, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("dir_init_bitmap: ",VERDE_L | BRILLANTE); printdword((dword) dir_init_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("dir_end_bitmap: ",VERDE_L | BRILLANTE); printdword((dword) dir_end_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("contarMemoria: ",VERDE_L | BRILLANTE); printdword((dword) &contarMemoria, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("cpmem: ",VERDE_L | BRILLANTE); printdword((dword) &cpmem, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//PAGINACION
	salto_de_linea();salto_de_linea();
	printf("PAGINACION", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("mapear_tabla: ",VERDE_L | BRILLANTE); printdword((dword) &mapear_tabla, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("liberar_directorio: ",VERDE_L | BRILLANTE); printdword((dword) &liberar_directorio, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//PANTALLA
	salto_de_linea();salto_de_linea();
	printf("PANTALLA", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("puntero_pantalla: ",VERDE_L | BRILLANTE); printdword((dword) &puntero_pantalla, BASE16 | VERDE_L | BRILLANTE);	
	salto_de_linea();
	printf("tarea_en_pantalla: ",VERDE_L | BRILLANTE); printdword((dword) &tarea_en_pantalla, BASE16 | VERDE_L | BRILLANTE);
/*	salto_de_linea();*/
/*	printf("tarea_a_mostrar: ",VERDE_L | BRILLANTE); printdword((dword) &tarea_a_mostrar, BASE16 | VERDE_L | BRILLANTE);*/
	salto_de_linea();
	printf("avanzar_puntero: ",VERDE_L | BRILLANTE); printdword((dword) &avanzar_puntero, BASE16 | VERDE_L | BRILLANTE);
/*	salto_de_linea();*/
/*	printf("cambiar_de_pantalla: ",VERDE_L | BRILLANTE); printdword((dword) &cambiar_de_pantalla, BASE16 | VERDE_L | BRILLANTE);*/
	
	
	breakpoint();
	clear_screen();
	
	
	//SCHEDULER
	printf("SCHEDULER", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("salto: ",VERDE_L | BRILLANTE); printdword((dword) &salto, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("switch_task: ",VERDE_L | BRILLANTE); printdword((dword) &switch_task, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//TECLADO
	salto_de_linea();salto_de_linea();
	printf("TECLADO", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("teclado[0]: ",VERDE_L | BRILLANTE); printdword((dword) &teclado[0], BASE16 | VERDE_L | BRILLANTE);
	printf("    teclado[126]: ",VERDE_L | BRILLANTE); printdword((dword) &teclado[126], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("getChar: ",VERDE_L | BRILLANTE); printdword((dword) &getChar, BASE16 | VERDE_L | BRILLANTE);

	
	//TSS
	salto_de_linea();salto_de_linea();
	printf("TSS", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("TSS[0]: ",VERDE_L | BRILLANTE); printdword((dword) &TSS[0], BASE16 | VERDE_L | BRILLANTE);
	printf("      TSS[49]: ",VERDE_L | BRILLANTE); printdword((dword) &TSS[49], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("crear_TSS: ",VERDE_L | BRILLANTE); printdword((dword) &crear_TSS, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("vaciar_TSS: ",VERDE_L | BRILLANTE); printdword((dword) &vaciar_TSS, BASE16 | VERDE_L | BRILLANTE);
	
	breakpoint();
	clear_screen();
}

































/**********************/
/*  MEMORIA AVANZADA  */
/**********************/


/* FALTA TESTEARLAS EHHH! */


void crear_heap(Heap* heap_dir, dword cant_bytes){

//si es un proceso de usuario, limitar la cant_bytes a un 2% del total de la memoria.

	dword primer_entrada = 0x7FFFFFF & cant_bytes; //por las dudas es a lo sumo 2^15-1
	
	*heap_dir = (Heap){
					(dword) primer_entrada,
					(Entry_Heap *) (((dword) heap_dir) + sizeof(Heap)),
					(Entry_Heap *) (((dword) heap_dir) + sizeof(Heap) + 4)
				};
	*(heap_dir->init) = primer_entrada;
}

void* pedir_memoria(Heap* heap_dir, dword cant_bytes){

	Entry_Heap* aux;
	void* res = NULL;
	dword resto;
	
	// si hay espacio para el pedido
	if(heap_dir->bytes_disponibles >= cant_bytes){
		
		// me fijo en que entrada del heap entra el pedido
		aux = devolverDondeEntre(heap_dir, cant_bytes);
		
		// si encontró espacio
		if(aux != NULL){
			resto = *aux - cant_bytes;
			
			//marco como ocupado
			*aux = *aux | 0x80000000;
			
			//si lo que sobra no alcanza para agregar una entry_heap
			if(resto < 5){
				aux =  (Entry_Heap *) (((dword) aux) + 4); //avanzo al byte luego del entry_heap
			}
			else{//si puedo agregar otra entry_heap a continuacion
				
				//paso al proximo entry_heap
				aux = (Entry_Heap *) (((dword) aux) + 4 + cant_bytes);
				
				//lo marco como libre y le pongo la cant de bytes libres a continuacion
				*aux = 0x7FFFFFFF & (resto-4);//notar que resto 4 por el tamaño que ocupa el entry_heap
				
				//si estoy apuntando al ultimo, corro al ultimo un lugar
				if(aux == heap_dir->end)	heap_dir->end++;
			}
			
			res	= (void *) aux;
		}				
	}
	
	return res;
}

Entry_Heap* devolverDondeEntre(Heap* heap_dir, dword cant_bytes){

	Entry_Heap *actual,*posible;
	dword tam_actual, tam_posible, ocupado;
	
	actual = heap_dir->init;	
	tam_actual = *actual & 0x7FFFFFFF;
	
	posible = actual;
	tam_posible = tam_actual;
	
	do{	
		//actualizo el tamaño y el estado
		tam_actual = *actual & 0x7FFFFFFF;
		ocupado = *actual & 0x80000000; //  0 = libre;

		if (ocupado == 0 && tam_actual > cant_bytes) 
		{
			//si tengo un mejor candidato
			if(tam_actual > tam_posible){
				posible = actual;
				tam_posible = tam_actual;
			}

		}
		if (ocupado == 0 && tam_actual == cant_bytes) {
			posible = actual;
			tam_posible = tam_actual;
			actual = heap_dir->end;
		}
		
		//si no llegue al final
		if(actual != heap_dir->end){
			//avanzo a la sig entrada
			actual = (Entry_Heap *) ( ((dword) actual) + 4 + tam_actual);
		}
	
	}while(actual != heap_dir->end);
	
	// si esta ocupado o no alcanza el lugar (aca entra solo si "posible" quedó apuntando al primer lugar del heap y no entra alli el pedido)
	if( ((*posible & 0x80000000) != 0) || ((*posible & 0x7FFFFFFF) < cant_bytes) ) posible = NULL;
	
	return posible;
}



void liberar_memoria(Heap* heap_dir, void* dir){}
