#include "../tipos/tipos_basicos.h"
#include "paginacion.h"
#include "../memoria/memoria.h"
#include "../pantalla/pantalla.h"

extern dword memoria_total;


// directorio_entry es la direccion de la entrada de directorio a la cual corresponde la dir_tabla
void mapear_tabla (dword *directorio_entry, dword dir_tabla, word atributos){
	*directorio_entry = (dir_tabla & 0xFFFFF000) | (atributos & 0x00000FFF);
}

// entry es la direccion del directorio
void mapear_pagina(dword *entry, dword dir_virtual, dword dir_real, word atributos){
	
	dword offset = 0;
	
	//muevo el puntero del directorio a la entrada del directorio que corresponda
	offset = dir_virtual / OFFSET_TABLA;
	entry += offset;
	
	//me fijo si la tabla esta presente o no, y si no lo esta agrego una tabla de pagina
	//notar que esto no se usa para la paginacion del kernel
	if( (*entry & 0x00000001) != (dword) PRESENT){
		dword* tabla = pidoPagina();
		mapear_tabla(entry, (dword) tabla, atributos);
	}
	
	//muevo el puntero a la tabla que corresponda
	entry = (dword *) ( (*entry) & 0xFFFFF000);
	
	//muevo el puntero a la entrada de tabla que corresponda
	offset = dir_virtual / OFFSET_TABLA;
	dir_virtual = dir_virtual - (offset * OFFSET_TABLA);
	offset = dir_virtual / OFFSET_PAGINA;
	entry += offset;
	
	*entry = (dir_real & 0xFFFFF000) | (atributos & 0x00000FFF);
}


void iniciar_paginacion_kernel(){
	dword *dir_entry = (dword *) DIR_DIRECTORIO;
	dword tabla_entry = (DIR_DIRECTORIO + 0x1000);
	dword dir_virtual_y_real = 0;
	
	dword cant_entradas_directorio = memoria_total / 4;//4MB por entrada de directorio
	dword cant_entradas_tabla;
	
	while(cant_entradas_directorio > 0){
	
		//aca mapeo las entradas del directorio a las tablas
		cant_entradas_tabla = 1024;
		mapear_tabla(dir_entry, tabla_entry, SUPERVISOR | WRITE | PRESENT);
		
		while(cant_entradas_tabla > 0){
			//mapeo las tablas a paginas fisicas con identity mapping
			mapear_pagina((dword *) DIR_DIRECTORIO, dir_virtual_y_real, dir_virtual_y_real, SUPERVISOR | WRITE | PRESENT);
			cant_entradas_tabla--;
			dir_virtual_y_real += 0x1000;
		}
		
		//apunto a la proxima tabla de pagina
		tabla_entry += 0x1000;
		cant_entradas_directorio--;
		dir_entry++;
	}
}


void liberar_directorio(dword* dir_entry){}
