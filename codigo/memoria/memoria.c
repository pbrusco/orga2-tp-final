#include "../tipos/tipos_basicos.h"
#include "memoria.h"

void contarMemoria()
{
	// Arranco suponiendo que el sistema cuenta con al menos 2 MB que es lo que ocupa el kernel. Si esto no fuese así, entonces el hardware no cumple con los requerimientos mínimos para correr el kernel.
	uint32 *dir = (uint32 *) (0x2*MB);
	uint32 dato = 0x12345678;

	*dir = dato;

	// Mientras el puntero sea menor que ?? y el contenido de la direccion apuntada sea la marca que se estableció previamente, recorro la memoria poniendo en cero la posición actual y marcando con el dato la posición siguiente.
	while (((uint32) dir < (32*MB)) &&  *dir == dato)
	{
		*dir = 0;
		dir += 1024;
		*dir = dato;
	}

	// Una vez fuera del ciclo, el puntero dir apunta al total de la memoria (en bytes).

	// Calculo cuanto es la memoria total en MB
	memoria_total = ((uint32) dir) / (1*MB);

	// Calculo el total de páginas libres calculando el total de páginas y excluyendo la cantidad de páginas ocupadas por el kernel.
	paginas_libres = ( (memoria_total * MB) - (2*MB) ) / TAM_PAG ;
}


void llenarBitmap()
{
	// Calculo cuantas paginas ocupa el directorio y las tablas de paginas del kernel
	uint64 offset_tablas = (memoria_total / 4) + 1;

	// Calculo donde arranca el bitmap, justo debajo de las tablas de paginas
	dir_init_bitmap = (uint8 *) DIR_DIRECTORIO + (offset_tablas * TAM_PAG);

	// Calculo donde termina el bitmap (ultima dir valida del bitmap)
	dir_end_bitmap = (uint8 *) (((uint32) dir_init_bitmap) + ((((memoria_total * MB) / TAM_PAG)/8) - 1));

	// Calculo cuantas páginas ocupa el kernel dividiendo la dirección donde termina por el tamaño de una página. Luego, calculo cuantos bytes representa esa cantidad dividiendo por 8.
	int32 pag_ocupadas = ((DIR_KERNEL_END) / TAM_PAG)/8;

	// Puntero auxiliar que apunta al comienzo del bitmap
	uint8* bitmap = dir_init_bitmap;

	// Ciclo que marca con un 1 en el Bitmap las páginas ocupadas.
	while(0 < pag_ocupadas)
	{
		*bitmap = 0xFF;
		bitmap++;
		pag_ocupadas--;
	}

	// Ciclo que marca con 0 en el Bitmap las páginas libres.
	while(bitmap <= dir_end_bitmap)
	{
		*bitmap = 0x00;
		bitmap++;
	}
}


uint32* pidoPagina()
{
	uint8* bitmap_dir = dir_init_bitmap + (((2*MB)/TAM_PAG)/8);//comienzo del bitmap + offset ocupado
	uint8 libre;
	uint32 contador_paginas = 512;//desde los 2 MB en adelante
	uint32 var = 0;

	while(bitmap_dir <= dir_end_bitmap && *bitmap_dir == 0xFF)
	{
		bitmap_dir++;
		contador_paginas = contador_paginas + 8;
	}

	if(bitmap_dir <= dir_end_bitmap)
	{
		libre = *bitmap_dir;

		int32 i;
		for(i = 0;i<8;i++)
		{
			if ((libre & 1) == 0)
			{
				var = contador_paginas + i;
				*bitmap_dir = *bitmap_dir | (1 << i); //Se actualiza el BIT correspondiente a la pagina obtenida

				//actualizo la cant de paginas libres
				paginas_libres--;
				return (uint32 *) (var*4*1024);
			}
			libre = libre >> 1;
		}
	}

	return 0;
}


void liberoPagina(uint32* pagina){

	if( ((uint32) pagina) >= 2*MB){
		//me guardo el numero de la pagina que se quiere liberar, comenzando a contar desde los 2MB
		uint32 numero_de_pagina = (((uint32) pagina) / TAM_PAG) - ( (uint32) (2*MB / TAM_PAG) );

		//comienzo del bitmap + offset ocupado
		uint8* bitmap_dir = dir_init_bitmap + (((2*MB)/TAM_PAG)/8);

		//muevo el puntero a donde corresponde y me fijo que pagina dentro de ese puntero debo liberar
		bitmap_dir += numero_de_pagina/8;
		numero_de_pagina = numero_de_pagina - (8*(numero_de_pagina/8));

		uint8 mascara = 0x01;
		mascara = mascara << numero_de_pagina;

		//actualizo el valor del bitmap, liberando la pagina
		*bitmap_dir = *bitmap_dir & (mascara ^ 0xFF); // el operador ^ es el XOR, con FF invierto todos los bits

		//actualizo la cant de paginas libres
		paginas_libres++;
	}
}


void setmem(uint8* dir, uint8 set, uint32 cant)
{
	while(cant > 0)
	{
		*dir = set;
		dir++;
		cant--;
	}
}


void cpmem(uint8* from, uint8* to, uint32 cant)
{
	while(cant > 0)
	{
		*to = *from;
		to++; from++;
		cant--;
	}
}







/***********************************************************************/
/*********ESTO ES PARA VER A DONDE ESTA UBICADO EL KERNEL ENTERO********/
/***********************************************************************/
#include "../pantalla/pantalla.h"
#include "../bcp/bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"
#include "../interrupciones/idt.h"
#include "../interrupciones/isr.h"
#include "../paginacion/paginacion.h"
#include "../scheduler/scheduler.h"
#include "../teclado/teclado.h"
#include "../applications/console.h"
#include "../kernel/kernel.h"

extern Info_Tareas tareas_en_memoria[];
extern int8 command[];
extern Tss TSS[];

extern uint8 tarea_actual, cant_tareas_en_sistema;

extern IDT_Descriptor IDT_DESC;
extern IDT_Entry IDT[];

extern uint16* puntero_pantalla;
extern uint8 tarea_en_pantalla, tarea_a_mostrar;



extern switch_reg salto;
extern int8 teclado[];
extern gdt_entry gdt_vector[];


void donde_esta_el_kernel(){
	clear_screen();
	//CONSOLE
	printf("CONSOLA", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("consoleD: ",VERDE_L | BRILLANTE); printdword((uint32) &console, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("extraerNumero: ",VERDE_L | BRILLANTE); printdword((uint32) &extract_number, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("tareas_en_memoria_info(var): ",VERDE_L | BRILLANTE); printdword((uint32) &tareas_en_memoria, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("command[0]: ",VERDE_L | BRILLANTE); printdword((uint32) &command, BASE16 | VERDE_L | BRILLANTE);
	printf("      command[99]: ",VERDE_L | BRILLANTE); printdword((uint32) &command[99], BASE16 | VERDE_L | BRILLANTE);

	//BCP
	salto_de_linea();salto_de_linea();
	printf("BCP", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("tarea_actual: ",VERDE_L | BRILLANTE); printdword((uint32) &tarea_actual, BASE16 | VERDE_L | BRILLANTE);	
	salto_de_linea();
	printf("cant_tareas_en_sistema: ",VERDE_L | BRILLANTE); printdword((uint32) &cant_tareas_en_sistema, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("iniciar_BCP: ",VERDE_L | BRILLANTE); printdword((uint32) &iniciar_BCP, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("desaparecerTarea: ",VERDE_L | BRILLANTE); printdword((uint32) &desaparecerTarea, BASE16 | VERDE_L | BRILLANTE);
	
	
	//GDT
	salto_de_linea();salto_de_linea();
	printf("GDT", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("gdt_vector[0]: ",VERDE_L | BRILLANTE); printdword((uint32) &gdt_vector[0], BASE16 | VERDE_L | BRILLANTE);
	printf("      gdt_vector[127]: ",VERDE_L | BRILLANTE); printdword((uint32) &gdt_vector[127], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("make_descriptor: ",VERDE_L | BRILLANTE); printdword((uint32) &make_descriptor, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("borrar_gdt_entry: ",VERDE_L | BRILLANTE); printdword((uint32) &borrar_gdt_entry, BASE16 | VERDE_L | BRILLANTE);
	
	//INTERRUPCIONES
	salto_de_linea();salto_de_linea();
	printf("INTERRUPCIONES", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("idt_fill: ",VERDE_L | BRILLANTE); printdword((uint32) &idtFill, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("idt[0]: ",VERDE_L | BRILLANTE); printdword((uint32) &IDT[0], BASE16 | VERDE_L | BRILLANTE);
	printf("      idt[255]: ",VERDE_L | BRILLANTE); printdword((uint32) &IDT[255], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("_INT0: ",VERDE_L | BRILLANTE); printdword((uint32) &_INT0 , BASE16 | VERDE_L | BRILLANTE);
	printf("      _INT80: ",VERDE_L | BRILLANTE); printdword((uint32) &_INT80 , BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("IDT_DESC: ",VERDE_L | BRILLANTE); printdword((uint32) &IDT_DESC, BASE16 | VERDE_L | BRILLANTE);
	
	
	breakpoint();
	clear_screen();
	
	
	//KERNEL
	printf("KERNEL", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("kernel_infinito: ",VERDE_L | BRILLANTE); printdword((uint32) &kernel_infinito, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("update_cursor: ",VERDE_L | BRILLANTE); printdword((uint32) &update_cursor, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//MEMORIA
	salto_de_linea();salto_de_linea();
	printf("MEMORIA", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("memoria_total: ",VERDE_L | BRILLANTE); printdword((uint32) &memoria_total, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("&dir_init_bitmap: ",VERDE_L | BRILLANTE); printdword((uint32) &dir_init_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("&dir_end_bitmap: ",VERDE_L | BRILLANTE); printdword((uint32) &dir_end_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("paginas_libres: ",VERDE_L | BRILLANTE); printdword((uint32) &paginas_libres, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("dir_init_bitmap: ",VERDE_L | BRILLANTE); printdword((uint32) dir_init_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("dir_end_bitmap: ",VERDE_L | BRILLANTE); printdword((uint32) dir_end_bitmap, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("contarMemoria: ",VERDE_L | BRILLANTE); printdword((uint32) &contarMemoria, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("cpmem: ",VERDE_L | BRILLANTE); printdword((uint32) &cpmem, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//PAGINACION
	salto_de_linea();salto_de_linea();
	printf("PAGINACION", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("mapear_tabla: ",VERDE_L | BRILLANTE); printdword((uint32) &mapear_tabla, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("liberar_directorio: ",VERDE_L | BRILLANTE); printdword((uint32) &liberar_directorio, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//PANTALLA
	salto_de_linea();salto_de_linea();
	printf("PANTALLA", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("puntero_pantalla: ",VERDE_L | BRILLANTE); printdword((uint32) &puntero_pantalla, BASE16 | VERDE_L | BRILLANTE);	
	salto_de_linea();
	printf("tarea_en_pantalla: ",VERDE_L | BRILLANTE); printdword((uint32) &tarea_en_pantalla, BASE16 | VERDE_L | BRILLANTE);
/*	salto_de_linea();*/
/*	printf("tarea_a_mostrar: ",VERDE_L | BRILLANTE); printdword((uint32) &tarea_a_mostrar, BASE16 | VERDE_L | BRILLANTE);*/
	salto_de_linea();
	printf("avanzar_puntero: ",VERDE_L | BRILLANTE); printdword((uint32) &avanzar_puntero, BASE16 | VERDE_L | BRILLANTE);
/*	salto_de_linea();*/
/*	printf("cambiar_de_pantalla: ",VERDE_L | BRILLANTE); printdword((uint32) &cambiar_de_pantalla, BASE16 | VERDE_L | BRILLANTE);*/
	
	
	breakpoint();
	clear_screen();
	
	
	//SCHEDULER
	printf("SCHEDULER", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("salto: ",VERDE_L | BRILLANTE); printdword((uint32) &salto, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("switch_task: ",VERDE_L | BRILLANTE); printdword((uint32) &switch_task, BASE16 | VERDE_L | BRILLANTE);
	
	
	
	//TECLADO
	salto_de_linea();salto_de_linea();
	printf("TECLADO", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("teclado[0]: ",VERDE_L | BRILLANTE); printdword((uint32) &teclado[0], BASE16 | VERDE_L | BRILLANTE);
	printf("    teclado[126]: ",VERDE_L | BRILLANTE); printdword((uint32) &teclado[126], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("getChar: ",VERDE_L | BRILLANTE); printdword((uint32) &getChar, BASE16 | VERDE_L | BRILLANTE);

	
	//TSS
	salto_de_linea();salto_de_linea();
	printf("TSS", ROJO_L | BRILLANTE);
	salto_de_linea();
	printf("TSS[0]: ",VERDE_L | BRILLANTE); printdword((uint32) &TSS[0], BASE16 | VERDE_L | BRILLANTE);
	printf("      TSS[49]: ",VERDE_L | BRILLANTE); printdword((uint32) &TSS[49], BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("crear_TSS: ",VERDE_L | BRILLANTE); printdword((uint32) &crear_TSS, BASE16 | VERDE_L | BRILLANTE);
	salto_de_linea();
	printf("vaciar_TSS: ",VERDE_L | BRILLANTE); printdword((uint32) &vaciar_TSS, BASE16 | VERDE_L | BRILLANTE);
	
	breakpoint();
	clear_screen();
}

































/**********************/
/*  MEMORIA AVANZADA  */
/**********************/


/* FALTA TESTEARLAS EHHH! */


void crear_heap(Heap* heap_dir, uint32 cant_bytes){

//si es un proceso de usuario, limitar la cant_bytes a un 2% del total de la memoria.

	uint32 primer_entrada = 0x7FFFFFF & cant_bytes; //por las dudas es a lo sumo 2^15-1

	*heap_dir = (Heap){
					(uint32) primer_entrada,
					(Entry_Heap *) (((uint32) heap_dir) + sizeof(Heap)),
					(Entry_Heap *) (((uint32) heap_dir) + sizeof(Heap) + 4)
				};
	*(heap_dir->init) = primer_entrada;
}

void* pedir_memoria(Heap* heap_dir, uint32 cant_bytes){

	Entry_Heap* aux;
	void* res = NULL;
	uint32 resto;

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
				aux =  (Entry_Heap *) (((uint32) aux) + 4); //avanzo al uint8 luego del entry_heap
			}
			else{//si puedo agregar otra entry_heap a continuacion

				//paso al proximo entry_heap
				aux = (Entry_Heap *) (((uint32) aux) + 4 + cant_bytes);

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

Entry_Heap* devolverDondeEntre(Heap* heap_dir, uint32 cant_bytes){

	Entry_Heap *actual,*posible;
	uint32 tam_actual, tam_posible, ocupado;

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
			actual = (Entry_Heap *) ( ((uint32) actual) + 4 + tam_actual);
		}

	}while(actual != heap_dir->end);

	// si esta ocupado o no alcanza el lugar (aca entra solo si "posible" quedó apuntando al primer lugar del heap y no entra alli el pedido)
	if( ((*posible & 0x80000000) != 0) || ((*posible & 0x7FFFFFFF) < cant_bytes) ) posible = NULL;

	return posible;
}



void liberar_memoria(Heap* heap_dir, void* dir){}

