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

void* malloc(int cant_bytes){

	id_tarea id_actual = obtenerIdActual();
	
	header* tabla = obtenerDirTabla(id_actual);
	
	return obtenerDirDisponible(tabla, cant_bytes);
}


void* obtenerDirDisponible(header* tabla,int cant_bytes){

void* d = devolverDondeEntre(tabla,cant_bytes);

if(¬presente(d)) pidoPaginas(d,cant_bytes);

return d;
}


pidoPaginas(void* d,int cant_bytes){






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
