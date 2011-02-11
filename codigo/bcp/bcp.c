#include "bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"
#include "../interrupciones/idt.h"
#include "../interrupciones/isr.h"
#include "../memoria/memoria.h"
#include "../paginacion/paginacion.h"
#include "../pantalla/pantalla.h"

extern gdt_entry gdt[];
extern idt_entry idt[];

//declaro el arreglo de BCP's
BCP_Entry BCP[CANT_TAREAS];

void iniciar_BCP(){
	//variables globales
	tarea_actual = 0;
	cant_tareas_en_sistema = 1;
	
	//datos del kernel
	BCP[0].pid = 5;
	BCP[0].estado = ACTIVO;
	BCP[0].entrada_directorio = (dword *) DIR_DIRECTORIO;
	BCP[0].sig = BCP[0].ant = 0;
	BCP[0].pantalla = (word *) 0xB8000;
}

void iniciar_tss_kernel(){
    byte tssVacia = buscar_TSS_vacia();
    crear_TSS(tssVacia, DIR_DIRECTORIO, 0, 0, 0);
    gdt[buscar_entradaGDT_vacia()] = make_descriptor((dword) &TSS[tssVacia], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_0 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);
}


// LLAMAR SIEMPRE Y CUANDO YA SE HAYA EJECUTADO "iniciar_BCP"
void crear_entradaBCP(dword id, byte estado, dword* ent_directorio, word* video){
	word entrada = buscar_entradaBCP_vacia();
	BCP[entrada].pid = id;
	BCP[entrada].estado = ACTIVO;
	BCP[entrada].pantalla = video;
	BCP[entrada].entrada_directorio = ent_directorio;
	BCP[entrada].ant = BCP[(byte) tarea_actual].ant;
	BCP[entrada].sig = (byte) tarea_actual;
	BCP[BCP[(byte) tarea_actual].ant].sig = entrada;
	BCP[(byte) tarea_actual].ant = entrada;
	cant_tareas_en_sistema++;
} 


word buscar_entradaBCP_vacia(){

	word vacia = 1;
	
	while( (BCP[vacia].estado != MUERTO) && (vacia < CANT_TAREAS) ){
		vacia++;
	}
	if(vacia == CANT_TAREAS)
		return 0;
	else
		return vacia;
}

byte buscar_entradaBCP(word id){
	byte busca = 0;
	while( (BCP[(byte) busca].pid != id) && (busca < CANT_TAREAS) ){
		busca++;
	}
	return busca;
}


void cambiar_estado(word id, byte estado_nuevo){

	byte busca = buscar_entradaBCP(id);

	if(busca < CANT_TAREAS){
		BCP[(byte) busca].estado = estado_nuevo;
		if(estado_nuevo == MUERTO){
			BCP[BCP[(byte) busca].ant].sig = BCP[(byte) busca].sig;
			BCP[BCP[(byte) busca].sig].ant = BCP[(byte) busca].ant;
		}
	}
}


void cargarTarea(dword eip){

	
	// 1ro: averiguar direccion de la tarea y tamaño (en bytes). Por ahora OBSOLETO
	

	// 2do: crear un directorio y las tablas de paginas necesarias y mapearlas segun corresponda, una pagina para la pila
	// y otra para el video
	dword *directorio = pidoPagina();
	dword *pila = pidoPagina();
	dword base_pila = (dword) pila;
	base_pila += 0xfff;
	word *video = (word *) pidoPagina();

	//mapeo las paginas que quiero con identity mapping
	mapeo_paginas_default(directorio);
	mapear_pagina(directorio, eip, eip, PRESENT | READ_PAGINACION | USUARIO);
	mapear_pagina(directorio, (dword) 0xB8000, (dword) 0xB8000, PRESENT | WRITE | USUARIO);
	mapear_pagina(directorio, (dword) pila, (dword) pila, PRESENT | WRITE | USUARIO);


	// 3ro: crear una entrada de TSS e inicializarla
	byte pos_TSS = buscar_TSS_vacia();
	crear_TSS(pos_TSS, (dword) directorio, (dword) eip, BASIC_EFLAGS, base_pila);

	
	// 4to: crear una entrada en la GDT para la TSS creada antes y mapearla
	word pid = buscar_entradaGDT_vacia();
	gdt[pid] = make_descriptor((dword) &TSS[pos_TSS], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_3 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);

	
	// 5to: crear entrada de BCP e inicializarla
	crear_entradaBCP(pid, ACTIVO, directorio, video);
}


void mapeo_paginas_default(dword* directorio){
	mapear_pagina(directorio, (dword) gdt, (dword) gdt, PRESENT | READ_PAGINACION | USUARIO);
	mapear_pagina(directorio, (dword) &gdt[127], (dword) &gdt[127], PRESENT | READ_PAGINACION | USUARIO);
	mapear_pagina(directorio, (dword) idt, (dword) idt, PRESENT | READ_PAGINACION | USUARIO);
	mapear_pagina(directorio, (dword) &idt[255], (dword) &idt[255], PRESENT | READ_PAGINACION | USUARIO);
	mapear_pagina(directorio, (dword) &_isr0, (dword) &_isr0, PRESENT | READ_PAGINACION | USUARIO);
	mapear_pagina(directorio, (dword) &_isr21, (dword) &_isr21, PRESENT | READ_PAGINACION | USUARIO);
	mapear_pagina(directorio, (dword) &iniciar_BCP, (dword) &iniciar_BCP, PRESENT | READ_PAGINACION | USUARIO);
}


