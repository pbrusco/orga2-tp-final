#include "../tipos/tipos_basicos.h"
#include "paginacion.h"

extern dword memoria_total;

void mapear_entrada(dword *entry, dword dir, word atributos){

	*entry = (dir & 0xFFFFF000) | (atributos & 0x00000FFF);
}


void iniciar_paginacion_kernel(){

	dword dir = 0;
	dword *dir_entry = (dword *) DIR_DIRECTORIO;
	dword *tabla_entry = (dword *) (DIR_DIRECTORIO + 0x1000);

	
	dword cant_entradas_directorio = memoria_total / 4;//4MB por entrada de directorio
	dword cant_entradas_tabla;
	
	
	while(cant_entradas_directorio > 0){
	
		//aca mapeo las entradas del directorio a las tablas
		cant_entradas_tabla = 1024;
		mapear_entrada(dir_entry, (dword) tabla_entry, SUPERVISOR | WRITE | PRESENT);
		dir_entry++;
		while(cant_entradas_tabla > 0){
			//mapeo las tablas a paginas fisicas con identity mapping
			mapear_entrada(tabla_entry, dir, SUPERVISOR | WRITE | PRESENT);
			dir += TAM_PAG;
			tabla_entry ++;
			cant_entradas_tabla--;
		}

		cant_entradas_directorio--;
	}
}
