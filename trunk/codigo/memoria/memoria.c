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

extern char command[];
extern char levanto;
extern idt_descriptor IDT_DESC;
extern word* puntero_pantalla;
extern switch_reg salto;
extern char teclado[];
extern gdt_entry gdt_vector[];


void donde_esta_el_kernel(){
	mover_puntero(0,0);
	printf("                                          ",0,0, 0);
	mover_puntero(1,0);
	printf("                                          ",0,0, 0);
	mover_puntero(0,0);
	printf("consoleD: ",2,VERDE_L | BRILLANTE, (dword) &console);
	mover_puntero(1,0);
	printf("extraerNumero: ",2,VERDE_L | BRILLANTE, (dword) &extract_number);
	mover_puntero(2,0);
	printf("command[0]: ",2,VERDE_L | BRILLANTE, (dword) &command);
	printf("      command[99]: ",2,VERDE_L | BRILLANTE, (dword) &command);
	mover_puntero(3,0);
	printf("levanto(consola): ",2,VERDE_L | BRILLANTE, (dword) &levanto);
	mover_puntero(4,0);
	printf("tarea_actual: ",2,VERDE_L | BRILLANTE, (dword) &tarea_actual);
	mover_puntero(5,0);
	printf("cant_tareas_en_sistema: ",2,VERDE_L | BRILLANTE, (dword) &cant_tareas_en_sistema);
	mover_puntero(6,0);
	printf("gdt_vector[0]: ",2,VERDE_L | BRILLANTE, (dword) &gdt_vector);
	printf("      gdt_vector[127]: ",2,VERDE_L | BRILLANTE, (dword) &gdt_vector[127]);
	mover_puntero(7,0);
	printf("make_descriptor: ",2,VERDE_L | BRILLANTE, (dword) &make_descriptor);
	mover_puntero(8,0);
	printf("idt_fill: ",2,VERDE_L | BRILLANTE, (dword) (dword) &idtFill);
	mover_puntero(9,0);
	printf("idt[0]: ",2,VERDE_L | BRILLANTE, (dword) (dword) &idt[0]);
	printf("      idt[255]: ",2,VERDE_L | BRILLANTE, (dword) &idt[255]);
	mover_puntero(10,0);
	printf("_isr0: ",2,VERDE_L | BRILLANTE, (dword) &_isr0);
	printf("      _isr21: ",2,VERDE_L | BRILLANTE, (dword) &_isr21);
	mover_puntero(11,0);
	printf("IDT_DESC: ",2,VERDE_L | BRILLANTE, (dword) &IDT_DESC);
	mover_puntero(12,0);
	printf("paginas_libres: ",2,VERDE_L | BRILLANTE, (dword) &paginas_libres);
	mover_puntero(13,0);
	printf("pidoPagina: ",2,VERDE_L | BRILLANTE, (dword) &pidoPagina);
	mover_puntero(14,0);
	printf("mapear_pagina: ",2,VERDE_L | BRILLANTE, (dword) &mapear_pagina);
	mover_puntero(15,0);
	printf("printf: ",2,VERDE_L | BRILLANTE, (dword) &printf);		
	mover_puntero(16,0);
	printf("puntero_pantalla: ",2,VERDE_L | BRILLANTE, (dword) &puntero_pantalla);
	mover_puntero(17,0);
	printf("salto: ",2,VERDE_L | BRILLANTE, (dword) &salto);
	mover_puntero(18,0);
	printf("switch_task: ",2,VERDE_L | BRILLANTE, (dword) &switch_task);
	mover_puntero(19,0);
	printf("teclado: ",2,VERDE_L | BRILLANTE, (dword) teclado);
	mover_puntero(20,0);
	printf("getChar: ",2,VERDE_L | BRILLANTE, (dword) &getChar);
	mover_puntero(21,0);
	printf("TSS[0]: ",2,VERDE_L | BRILLANTE, (dword) &TSS);
	printf("      TSS[49]: ",2,VERDE_L | BRILLANTE, (dword) &TSS[49]);
	mover_puntero(22,0);
	printf("crear_TSS: ",2,VERDE_L | BRILLANTE, (dword) &crear_TSS);
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
