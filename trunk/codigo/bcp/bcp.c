#include "bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"
#include "../interrupciones/idt.h"
#include "../memoria/memoria.h"
#include "../paginacion/paginacion.h"

extern gdt_entry gdt[];

//declaro el arreglo de BCP's
BCP_Entry BCP[CANT_TAREAS];

void iniciar_BCP(){
	//variables globales
	tarea_actual = -1;
	cant_tareas_en_sistema = 0;
}

void iniciar_tss_kernel(){
    byte tssVacia = buscar_TSS_vacia();
    crear_TSS(tssVacia, DIR_DIRECTORIO, 0, 0, 0);
    gdt[buscar_entradaGDT_vacia()] = make_descriptor((dword) &TSS[tssVacia], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_0 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);
}



void crear_entradaBCP(word entrada, dword id, byte estado, dword* ent_directorio){
	BCP[entrada].pid = id;
	BCP[entrada].estado = ACTIVO;
	BCP[entrada].entrada_directorio = ent_directorio;
	if(cant_tareas_en_sistema != 0){
		BCP[entrada].ant = BCP[(byte) tarea_actual].ant;
		BCP[entrada].sig = (byte) tarea_actual;
		BCP[BCP[(byte) tarea_actual].ant].sig = entrada;
		BCP[(byte) tarea_actual].ant = entrada;
	}
	else{
		BCP[entrada].ant = entrada;
		BCP[entrada].sig = entrada;
	}
} 


word buscar_entradaBCP_vacia(){

	word vacia = 0;
	
	while(BCP[vacia].estado != MUERTO){
		vacia++;
	}

	return vacia;
}

//OJO: SE CUELGA SI NO EXISTE ID
void cambiar_estado(dword id, byte estado_nuevo){

	char busca = tarea_actual;

	while(BCP[(byte) busca].pid != id){
		busca++;
	}

	BCP[(byte) busca].estado = estado_nuevo;
	if(estado_nuevo == MUERTO){
		BCP[BCP[(byte) busca].ant].sig = BCP[(byte) busca].sig;
		BCP[BCP[(byte) busca].sig].ant = BCP[(byte) busca].ant;
	}
}



/**********************************************************************************
	LO DE ABAJO SON PRUEBAS QUE HICE PARA VER QUE TODO ANDE BIEN.
**********************************************************************************/




void cargarTarea1(){

	
	// 1ro: averiguar direccion de la tarea y tamaño (en bytes)
	
	dword *eip = (dword *) 0x2000;

	
	// 2do: crear un directorio y las tablas de paginas necesarias y mapearlas segun corresponda, y una pagina para la pila

	dword *directorio = pidoPagina();
	dword *table_entry = pidoPagina();
	dword *pila = pidoPagina();
	dword base_pila = (dword) pila;
	base_pila += 0xfff;

	//mapeo la primer entrada del directorio a la entrada de tabla que voy a armar
	mapear_tabla(directorio, (dword) table_entry, 0, PRESENT | READ_PAGINACION | USUARIO);

	//mapeo las paginas que quiero con identity mapping
	dword dir = 0;
	while(dir < 0x200000){
		mapear_pagina(directorio, (dword) dir, (dword) dir, PRESENT | READ_PAGINACION | USUARIO);
		dir += 0x1000;
	}
	mapear_pagina(directorio, (dword) 0xB8000, (dword) 0xB8000, PRESENT | WRITE | USUARIO);
	mapear_pagina(directorio, (dword) pila, (dword) pila, PRESENT | WRITE | USUARIO);



	// 3ro: crear una entrada de TSS e inicializarla

	byte pos_TSS = buscar_TSS_vacia();
	crear_TSS(pos_TSS, (dword) directorio, (dword) eip, BASIC_EFLAGS, base_pila);

	
	// 4to: crear una entrada en la GDT para la TSS creada antes y mapearla
	
	word pid = buscar_entradaGDT_vacia();
	gdt[pid] = make_descriptor((dword) &TSS[pos_TSS], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_3 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);

	
	// 5to: crear entrada de BCP e inicializarla
	
	word bcp_pos = buscar_entradaBCP_vacia();
	crear_entradaBCP(bcp_pos, pid, ACTIVO, (dword *) directorio);

}



void cargarTarea2(){

	
	// 1ro: averiguar direccion de la tarea y tamaño (en bytes)
	
	dword *eip = (dword *) 0x3000;

	
	// 2do: crear un directorio y las tablas de paginas necesarias y mapearlas segun corresponda, y una pagina para la pila
	

	dword *directorio = pidoPagina();
	dword *table_entry = pidoPagina();
	dword *pila = pidoPagina();
	dword base_pila = (dword) pila;
	base_pila += 0xfff;

	//mapeo la primer entrada del directorio a la entrada de tabla que voy a armar
	mapear_tabla(directorio, (dword) table_entry, 0, PRESENT | READ_PAGINACION | SUPERVISOR);

	//mapeo las paginas que quiero con identity mapping
	dword dir = 0;
	while(dir < 0x200000){
		mapear_pagina(directorio, (dword) dir, (dword) dir, PRESENT | READ_PAGINACION | SUPERVISOR);
		dir += 0x1000;
	}
	mapear_pagina(directorio, (dword) 0xB8000, (dword) 0xB8000, PRESENT | WRITE | SUPERVISOR);
	mapear_pagina(directorio, (dword) pila, (dword) pila, PRESENT | WRITE | SUPERVISOR);


	
	// 3ro: crear una entrada de TSS e inicializarla
	
	byte pos_TSS = buscar_TSS_vacia();
	crear_TSS(pos_TSS, (dword) directorio, (dword) eip, BASIC_EFLAGS, base_pila);

	
	// 4to: crear una entrada en la GDT para la TSS creada antes y mapearla
	
	word pid = buscar_entradaGDT_vacia();
	gdt[pid] = make_descriptor((dword) &TSS[pos_TSS], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_0 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);

	
	// 5to: crear entrada de BCP e inicializarla
	
	word bcp_pos = buscar_entradaBCP_vacia();
	crear_entradaBCP(bcp_pos, pid, ACTIVO, (dword *) directorio);

}

void arreglar(){
	TSS[0].es = 0x10;
}

