#include "../tipos/tipos_basicos.h"
#include "paginacion.h"
#include "../memoria/memoria.h"
#include "../pantalla/pantalla.h"

extern uint32 memoria_total;


// directorio_entry es la direccion de la entrada de directorio a la cual corresponde la dir_tabla
void mapear_tabla (uint32 *directorio_entry, uint32 dir_tabla, uint16 atributos){
	*directorio_entry = (dir_tabla & 0xFFFFF000) | (atributos & 0x00000FFF);
}

// entry es la direccion del directorio
void mapear_pagina(uint32 *entry, uint32 dir_virtual, uint32 dir_real, uint16 atributos){
	
	uint32 offset = 0;
	
	//muevo el puntero del directorio a la entrada del directorio que corresponda
	offset = dir_virtual / OFFSET_TABLA;
	entry += offset;
	
	//me fijo si la tabla esta presente o no, y si no lo esta agrego una tabla de pagina
	//notar que esto no se usa para la paginacion del kernel
	if( (*entry & 0x00000001) != (uint32) PRESENT){
		uint32* tabla = pidoPagina();
		mapear_tabla(entry, (uint32) tabla, atributos);
	}
	
	//si tengo que hacer una escritura y no tengo permisos para escribir, cambio los permisos
	if(    ((atributos & WRITE) == 2) && ((*entry & 0x2) != 0x2)    ){
		*entry = *entry | WRITE;
	}
	
	//muevo el puntero a la tabla que corresponda
	entry = (uint32 *) ( (*entry) & 0xFFFFF000);
	
	//muevo el puntero a la entrada de tabla que corresponda
	offset = dir_virtual / OFFSET_TABLA;
	dir_virtual = dir_virtual - (offset * OFFSET_TABLA);
	offset = dir_virtual / OFFSET_PAGINA;
	entry += offset;
	
	*entry = (dir_real & 0xFFFFF000) | (atributos & 0x00000FFF);
}


uint32 obtener_mapeo(uint32 *directorio, uint32 dir_virtual){
	
	uint32 offset = 0;
	
	//muevo el puntero del directorio a la entrada del directorio que corresponda
	offset = dir_virtual / OFFSET_TABLA;
	directorio += offset;
	
	//muevo el puntero a la tabla que corresponda
	directorio = (uint32 *) ( (*directorio) & 0xFFFFF000);
	
	//muevo el puntero a la entrada de tabla que corresponda
	dir_virtual = dir_virtual - (offset * OFFSET_TABLA);
	offset = dir_virtual / OFFSET_PAGINA;
	directorio += offset;
	
	return *directorio;
}


void iniciar_paginacion_kernel(){
	uint32 *dir_entry = (uint32 *) DIR_DIRECTORIO;
	uint32 tabla_entry = (DIR_DIRECTORIO + 0x1000);
	uint32 dir_virtual_y_real = 0;
	
	uint32 cant_entradas_directorio = memoria_total / 4;//4MB por entrada de directorio
	uint32 cant_entradas_tabla;
	
	while(cant_entradas_directorio > 0){
	
		//aca mapeo las entradas del directorio a las tablas
		cant_entradas_tabla = 1024;
		mapear_tabla(dir_entry, tabla_entry, SUPERVISOR | WRITE | PRESENT);
		
		while(cant_entradas_tabla > 0){
			//mapeo las tablas a paginas fisicas con identity mapping
			mapear_pagina((uint32 *) DIR_DIRECTORIO, dir_virtual_y_real, dir_virtual_y_real, SUPERVISOR | WRITE | PRESENT);
			cant_entradas_tabla--;
			dir_virtual_y_real += 0x1000;
		}
		
		//apunto a la proxima tabla de pagina
		tabla_entry += 0x1000;
		cant_entradas_directorio--;
		dir_entry++;
	}
}


void liberar_directorio(uint32* dir_entry){
	
	//declaro variables que voy a usar para indexar las tablas y el directorio
	uint16 indice_d;
	uint16 indice_t;
	uint32* table_entry;
	
	//para todas las entradas del directorio
	for(indice_d = 0; indice_d < 1024;indice_d++){
		
		//si la tabla de pagina esta presente
		if( (*dir_entry & 1) == PRESENT ){
	
			//inicio variables
			table_entry = (uint32*) (*dir_entry & 0xFFFFF000);
			
			//para cada entrada de la tabla
			for(indice_t = 0; indice_t < 1024;indice_t++){
				
				//si la pagina esta mapeada
				if( (*table_entry & 1) == PRESENT ){

					//limpio y libero la pagina, siempre que la misma este por encima de los 2MB
					if(*table_entry >= 2*MB){
						setmem((uint8*) (*table_entry & 0xFFFFF000), 0x00, TAM_PAG);
						liberoPagina((uint32*) *table_entry);
					}
				}
				table_entry++;
			}
			
			//luego de liberar y limpiar todo, limpio y libero la pagina usada para la tabla
			setmem((uint8*) (*dir_entry & 0xFFFFF000), 0x00, TAM_PAG);
			liberoPagina((uint32*) (*dir_entry & 0xFFFFF000));
		}
		dir_entry++;
	}
	
	//una vez limpias y libres todas las paginas y las tablas, limpio y libero el directorio
	dir_entry -= 1024;//notar que luego del for, queda apuntando al primer uint32 de la siguiente pagina
	setmem((uint8*) dir_entry, 0x00, TAM_PAG);
	liberoPagina(dir_entry);
}
