#include "bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"
#include "../memoria/memoria.h"
#include "../paginacion/paginacion.h"

extern tss TSS[];
extern gdt_entry gdt[];

void iniciar_BCP(){

	//variables globales
	tarea_actual = BCP;//la tarea BCP[0] es la del kernel
	cant_tareas_en_sistema = 1;
	
	
	BCP[0].pid = buscar_entradaGDT_vacia();//entrada TSS en la GDT donde estara el kernel
	BCP[0].estado = KERNEL;
	BCP[0].entrada_directorio = (dword *) DIR_DIRECTORIO;
	BCP[0].ant = &BCP[0];
	BCP[0].sig = &BCP[0];

}

void iniciar_tss_kernel(){
    tss* tssVacia = buscar_TSS_vacia();
    
    gdt[BCP[0].pid] = make_descriptor((dword) tssVacia, TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_0 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);
}



void crear_entrada(word entrada, dword id, byte estado, dword* ent_directorio){
	BCP_Entry* nuevo = &BCP[entrada];
	nuevo->pid = id;
	nuevo->estado = ACTIVO;
	nuevo->entrada_directorio = ent_directorio;
	nuevo->ant = tarea_actual->ant;
	nuevo->sig = tarea_actual;
	(tarea_actual->ant)->sig = nuevo;
	tarea_actual->ant = nuevo;
} 


word buscar_entradaBCP_vacia(){

	word vacia = 1;
	
	while(BCP[vacia].estado != MUERTO){
		vacia++;
	}

	return vacia;
}


void cambiar_estado(dword id, byte estado_nuevo){

	BCP_Entry *busca = tarea_actual;

	while(busca->pid != id){
		busca = busca->sig;
	}

	busca->estado = estado_nuevo;
	if(estado_nuevo == MUERTO){
		(busca->ant)->sig = busca->sig;
		(busca->sig)->ant = busca->ant;
	}
}


void cargarTarea(){

	/*
	1ro: averiguar direccion de la tarea y tamaño (en bytes)
	*/
	dword *dir = (dword *) 0x4000;
	dword tam = 4096;

	/*
	2do: crear un directorio y las tablas de paginas necesarias y mapearlas segun corresponda
	*/

	dword *directorio = pidoPagina();
	dword *table_entry = pidoPagina();
	
	//mapeo la primer entrada del directorio a la entrada de tabla que voy a armar
	mapear_entrada(directorio, (dword) table_entry, PRESENT | WRITE | USUARIO);

	//busco entradas a mapear en la tabla y las mapeo
	
	// a) busco y mapeo la direccion del programa
	table_entry += 4;
	mapear_entrada(table_entry, (dword) 0x4000, PRESENT | READ_PAGINACION | USUARIO);
	// b) busco y mapeo la direccion de video
	table_entry += 0xB4;
	mapear_entrada(table_entry, (dword) 0xB8000, PRESENT | WRITE | USUARIO);

	/*
	3ro: crear una entrada de TSS e inicializarla
	*/
	tss* pos_TSS = buscar_TSS_vacia();
	crear_TSS( (dword) pos_TSS, (dword) directorio, (dword) dir, BASIC_EFLAGS);

	/*
	4to: crear una entrada en la GDT para la TSS creada antes y mapearla
	*/
	word pid = buscar_entradaGDT_vacia();
	gdt[pid] = make_descriptor((dword) pos_TSS, TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_3 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);

	/*
	5to: crear entrada de BCP e inicializarla
	*/
	word bcp_pos = buscar_entradaBCP_vacia();
	crear_entrada(bcp_pos, pid, ACTIVO, directorio);

}

