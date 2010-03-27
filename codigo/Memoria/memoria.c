#include "tipos/tipos.h"




void llenarBitmap(){
	byte* bitmap = DIRECCION_BITMAP;
	for(int i = 0; i < 32;i++)
	{
		*bitmap = 0xFF;
		bitmap = bitmap + 1;
	}	
	for(int i = 32; i < 131072;i++)
	{
		*bitmap = 0x00;
		bitmap = bitmap + 1;
	}	
}

void* malloc(unsigned int cant_bytes){

	id_tarea id_actual = obtenerIdActual();
	
	header* tabla = obtenerDirTabla(id_actual);
	
	return obtenerDirDisponible(tabla, cant_bytes);
}


void* obtenerDirDisponible(header* tabla,unsigned int cant_bytes){

	void* d = devolverDondeEntre(tabla,cant_bytes);
	void* pagina_libre, d_aux;
	d_aux = d & 0xFFFFF000; //lo llevo al inicio de la pagina
	int cant_paginas = (cant_bytes-1)/4096 + 1;  //devuelve la cantidad de paginas que se usaran
	
	while(cant_paginas > 0){
		if(¬estaPresente(d_aux,cant_bytes)){
			pagina_libre = pidoPagina();
			remapear(pagina_libre,d_aux);
		}
		cant_paginas--;
		d_aux += 4096;
	}
	return d;
}


void* pidoPagina()
{

	byte* bitmap_dir;
	bitmap_dir = DIRECCION_BITMAP;
	byte libre;
	unsigned int contador_paginas = 0;

	while(*bitmap_dir == 0xFF){
		bitmap_dir++;
		contador_paginas = contador_paginas + 8;
	}

	libre = *bitmap_dir;
	 
	int i;
	for(i = 0;i<8;i++) {
		if (libre & 0x80 != 0)
		{
			var = contador_paginas + i;
			*bitmap_dir = *bitmap_dir || (0x01 << (7-i)); //Se actualiza el BIT correspondiente a la pagina obtenida
			return var*4*1024;
		}
		libre << 1;
		}
	return 0;
}




void* devolverDondeEntre(header* tabla,unsigned int cant_bytes){
	int tam,ocupado,fin,tam_acumulados;
	tam_acumulados = 0;
	header* posible = 0;
	
	do{
		tam = *tabla & 0x1FFFFFFF; //regresa el tamaño en el header
		ocupado = *tabla & 0x80000000; //  0 = libre;
		fin = *tabla & 0xE0000000; //0x0 = fin;

		if (ocupado == 0 && tam >= cant_bytes) 
		{
			posible = tabla;
			tam_posible = tam_acumulados;	
		}
		if (tam == cant_bytes) {
			*tabla || 0x8000000;
			return DIRECCION_BASE_MALLOC + tam_acumulados;
		}

		tam_acumulados = tam_acumulados + tam;
		tabla = tabla + 1;

	
	}while(fin != 0);
	
	
	if(tam > cant_bytes){
		*tabla = 0xE0000000 + cant_bytes;
		*(tabla+1) = tam - cant_bytes;
		return DIRECCION_BASE_MALLOC + tam_acumulados;
	}

	if(posible != 0) {
	while(tabla != posible) ...
	
	
	}



		
	}


}




/*
Pseudo de MALLOC:

	Busco en el arreglo de headers de LAS tareas (*) el puntero a los headers de LA tarea que pidio el malloc (**).

	While(no termina el header o no encuentra un lugar disponible)
		Busco y acumulo los tamaños recorridos.
	
	No encuentro!:
	ERR0

	Encuentro:
		-Calculo la direccion (d)
		-Me fijo si las paginas necesarias contenidas en el espacio pedido (desde d hasta d+tamaño) estan presentes
			En caso que no esten presentes, ver si se consiguen la cantidad necesaria, si no se consiguen ,ERR1
			Si se consiguen, REMAPEAR cada una para que se logren las necesitadas..
		-Actualizo los headers. (intento insertar en un header Libre con el mismo tamaño que el pedido, sino al final y si 			no hay mas espacio al final ,entonces busco donde meterlo y corro los headers necesarios)	
		
	
		ERR0 No hay mas memoria disponible para LA tarea.
		ERR1 No hay mas memoria en el sistema.
		(*) Arreglo que contiene Id de la tarea y direccion en la que se encuentran los headers de su memoria. (hasta 50)
		(**) "lista" o conjunto de headers consecutivos de 4 bytes cada uno que tienen 1 bit para libre u ocupado y 35 				para el tamaño (solo hacen falta 30)..	.
}
*/
