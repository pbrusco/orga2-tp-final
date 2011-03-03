#include "bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"
#include "../interrupciones/idt.h"
#include "../interrupciones/isr.h"
#include "../memoria/memoria.h"
#include "../paginacion/paginacion.h"
#include "../scheduler/scheduler.h"
#include "../pantalla/pantalla.h"

extern idt_entry idt[];
extern gdt_entry gdt_vector[];
extern byte tarea_en_pantalla;
extern byte tarea_a_mostrar;
extern byte command[];

//declaro el arreglo de BCP's
BCP_Entry BCP[CANT_TAREAS];

void iniciar_BCP(){
	//variables globales
	tarea_actual = 0;
	cant_tareas_en_sistema = 1;
	tarea_en_pantalla = 0;
	tarea_a_mostrar = 0;

	//datos del kernel
	BCP[0].pid = 5;
	BCP[0].estado = ACTIVO;
	BCP[0].entrada_directorio = (dword *) DIR_DIRECTORIO;
	BCP[0].sig = BCP[0].ant = 0;
	BCP[0].pantalla = (word *) 0xB9000;//TODO: agregar al mapa de memoria
}

void iniciar_tss_kernel(){
    byte tssVacia = buscar_TSS_vacia();
    crear_TSS(tssVacia, DIR_DIRECTORIO, 0, 0, 0, 0);
    gdt_vector[buscar_entradaGDT_vacia()] = make_descriptor((dword) &TSS[tssVacia], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_0 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);
}


// LLAMAR SIEMPRE Y CUANDO YA SE HAYA EJECUTADO "iniciar_BCP"
void crear_entradaBCP(dword id, byte estado, dword* ent_directorio, word* video){
	word entrada = buscar_entradaBCP_vacia();
	BCP[entrada].pid = id;
	BCP[entrada].estado = ACTIVO;
	BCP[entrada].pantalla = video;
	BCP[entrada].entrada_directorio = ent_directorio;
	BCP[entrada].ant = BCP[tarea_actual].ant;
	BCP[entrada].sig = tarea_actual;
	BCP[BCP[tarea_actual].ant].sig = entrada;
	BCP[tarea_actual].ant = entrada;
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
		BCP[busca].estado = estado_nuevo;
		if(estado_nuevo == MATAR){
			BCP[BCP[busca].ant].sig = BCP[busca].sig;
			BCP[BCP[busca].sig].ant = BCP[busca].ant;
			cant_tareas_en_sistema--;
		}
	}
}


void cargarTarea(dword eip){

	// 1ro: averiguar direccion de la tarea y tamaño (en bytes). Por ahora OBSOLETO


	// 2do: crear un directorio y las tablas de paginas necesarias y mapearlas segun corresponda, una pagina para la pila
	// y otra para el video
	dword *directorio = pidoPagina();
	dword *pila = pidoPagina();
	dword *pila0 = pidoPagina();
	word *video = (word *) pidoPagina();

	//mapeo las paginas que quiero con identity mapping
	mapeo_paginas_default(directorio);
	mapear_pagina(directorio, eip, eip, PRESENT | WRITE | USUARIO);
	mapear_pagina(directorio, (dword) pila, (dword) pila, PRESENT | WRITE | USUARIO);
	mapear_pagina(directorio, (dword) pila0, (dword) pila0, PRESENT | WRITE | SUPERVISOR);
	//mapeo la pagina de video a la pagina de video de la tarea
	mapear_pagina(directorio, (dword) 0xB8000, (dword) video, PRESENT | WRITE | USUARIO);


	// 3ro: crear una entrada de TSS e inicializarla
	byte pos_TSS = buscar_TSS_vacia();
	crear_TSS(pos_TSS, (dword) directorio, (dword) eip, BASIC_EFLAGS, ((dword)pila) + TAM_PAG, ((dword)pila0) + TAM_PAG);


	// 4to: crear una entrada en la GDT para la TSS creada antes y mapearla
	word pid = buscar_entradaGDT_vacia();
	gdt_vector[pid] = make_descriptor((dword) &TSS[pos_TSS], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_3 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);

	// 5to: crear entrada de BCP e inicializarla
	crear_entradaBCP(pid, ACTIVO, directorio, video);
}



void matarTarea(byte id){
	//limpio las interrupciones para evitar problemas
	cli();

	cambiar_estado(id, MATAR);

	//activo interrupciones nuevamente
	sti();
}



void mapeo_paginas_default(dword* directorio){
	
	//pagina en donde empieza el codigo del kernel
	dword kernel_init = ((dword) &make_descriptor) & 0xFFFFF000;
	
	//pagina siguiente a la pagina donde termina el kernel
	dword kernel_end = ( ((dword) &command[99]) & 0xFFFFF000) + TAM_PAG;
	
	//mapeo la pagina en donde esta la estructura de la GDT
	mapear_pagina(directorio, (dword) gdt_vector, (dword) gdt_vector, PRESENT | READ_PAGINACION | USUARIO);
	
	//mapeo todas las paginas en donde se encuentra el kernel
	while(kernel_init != kernel_end){
		mapear_pagina(directorio, kernel_init, kernel_init, PRESENT | READ_PAGINACION | USUARIO);
		kernel_init += TAM_PAG;
	}
		
}



byte buscar_entradaBCP_matar(){

	byte res = 0;
	while( (res < CANT_TAREAS) && (BCP[res].estado != MATAR) ){
		res++;
	}
	
	return res; //NOTA: devuelve la posicion de la tarea en la BCP
}


void info_BCP(byte index){
	mover_puntero(10,0);
	printf("Entrada BCP: ", CELESTE_L); printdword(index, CELESTE_L); printf("\n",0);
	printf("pid: ", GRIS_L | BRILLANTE); printdword(BCP[index].pid, GRIS_L | BRILLANTE); printf("\n",0);
	printf("estado: ", GRIS_L | BRILLANTE); printdword(BCP[index].estado, GRIS_L | BRILLANTE); printf("\n",0);
	printf("directorio: ", GRIS_L | BRILLANTE); printdword((dword) BCP[index].entrada_directorio, GRIS_L | BRILLANTE); printf("\n",0);
	printf("siguiente: ", GRIS_L | BRILLANTE); printdword(BCP[index].sig, GRIS_L | BRILLANTE); printf("\n",0);
	printf("anterior: ", GRIS_L | BRILLANTE); printdword(BCP[index].ant, GRIS_L | BRILLANTE); printf("\n",0);
	printf("pantalla: ", GRIS_L | BRILLANTE); printdword((dword) BCP[index].pantalla, GRIS_L | BRILLANTE); printf("\n",0);
}



void desaparecerTarea(byte bcpPos){

	//pongo en 0 y libero las paginas que se pidieron para el directorio y las tablas de pagina
	liberar_directorio(BCP[bcpPos].entrada_directorio);

	//recupero la direccion de la tss
	tss* task_tss = (tss*) ( ( ((dword) gdt_vector[BCP[bcpPos].pid].base2) << 16 ) |\
				 ( (word) gdt_vector[BCP[bcpPos].pid].base1) );

	//recupero la dir de la pila
	byte* task_pila = (byte*) (task_tss->ebp & 0xFFFFF000);

	//pongo en 0 y libero pagina de la pila
	setmem(task_pila,0x00,TAM_PAG);
	liberoPagina((dword*) task_pila);

	//pongo en 0 y libero pagina del video
	setmem((byte*) BCP[bcpPos].pantalla,0x00,80*24*2);
	liberoPagina((dword*) BCP[bcpPos].pantalla);

	//libero la tss (OJO: solo pone en 0 el cr3, pero por ahora es suficiente)
	vaciar_TSS(task_tss);

	//libero la entrada de GDT
	gdt_vector[BCP[bcpPos].pid] = make_descriptor(0,0,0,0);

	//libero la entrada del BCP
	BCP[bcpPos].estado = MUERTO;
}

