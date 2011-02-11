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
	dword numero_de_pagina = (((int) pagina) / TAM_PAG) - (2*MB / TAM_PAG);
	
	//comienzo del bitmap + offset ocupado
	byte* bitmap_dir = dir_init_bitmap + (((2*MB)/TAM_PAG)/8);

	while(numero_de_pagina > 7){
		numero_de_pagina -= 8;
		bitmap_dir++;
	}
	
	byte mascara = 0x01;
	while(numero_de_pagina > 0){
		numero_de_pagina--;
		mascara = mascara << 1;
	}

	*bitmap_dir = *bitmap_dir & (mascara ^ 0xFF); // el operador ^ es el XOR, con FF invierto todos los bits
	
	//actualizo la cant de paginas libres
	paginas_libres++;
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
