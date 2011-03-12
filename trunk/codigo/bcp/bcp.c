#include "../tipos/tipos_basicos.h"
#include "bcp.h"
#include "../gdt/gdt.h"
#include "../tss/tss.h"
#include "../interrupciones/idt.h"
#include "../interrupciones/isr.h"
#include "../memoria/memoria.h"
#include "../paginacion/paginacion.h"
#include "../scheduler/scheduler.h"
#include "../pantalla/pantalla.h"

extern IDT_Entry IDT[];
extern gdt_entry gdt_vector[];
extern uint8 tarea_en_pantalla,tarea_a_mostrar;
extern uint8 command[];
extern Tss TSS[];


//variables globales
uint8 tarea_actual;
uint8 cant_tareas_en_sistema;

//declaro el arreglo de BCP's
BCP_Entry BCP[CANT_TAREAS];

void iniciar_BCP(){
	//variables globales
	tarea_actual = 0;
	cant_tareas_en_sistema = 1;
	tarea_en_pantalla = 5;
	tarea_a_mostrar = 5;

	//datos del kernel
	BCP[0].pid = 5;
	BCP[0].estado = ACTIVO;
	BCP[0].entrada_directorio = (uint32 *) DIR_DIRECTORIO;
	BCP[0].sig = BCP[0].ant = 0;
	BCP[0].pantalla = (uint16 *) 0xB9000;//TODO: agregar al mapa de memoria
	BCP[0].nombre = "kernel";
}

void iniciar_tss_kernel(){
    uint8 tssVacia = buscar_TSS_vacia();
    crear_TSS(tssVacia, DIR_DIRECTORIO, 0, 0, 0, 0);
    gdt_vector[buscar_entradaGDT_vacia()] = make_descriptor((uint32) &TSS[tssVacia], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_0 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);
}


// LLAMAR SIEMPRE Y CUANDO YA SE HAYA EJECUTADO "iniciar_BCP"
void crear_entradaBCP(uint32 id, uint8 estado, uint32* ent_directorio, uint16* video, int8* name){
	uint16 entrada = buscar_entradaBCP_vacia();
	BCP[entrada].pid = id;
	BCP[entrada].estado = ACTIVO;
	BCP[entrada].pantalla = video;
	BCP[entrada].entrada_directorio = ent_directorio;
	BCP[entrada].nombre = name;

	BCP[entrada].ant = BCP[tarea_actual].ant;
	BCP[entrada].sig = tarea_actual;
	BCP[BCP[tarea_actual].ant].sig = entrada;
	BCP[tarea_actual].ant = entrada;
	cant_tareas_en_sistema++;
}


uint16 buscar_entradaBCP_vacia(){

	uint16 vacia = 1;

	while( (BCP[vacia].estado != MUERTO) && (vacia < CANT_TAREAS) ){
		vacia++;
	}
	if(vacia == CANT_TAREAS)
		return 0;
	else
		return vacia;
}

uint8 buscar_entradaBCP(uint16 id){
	uint8 busca = 0;
	while( (BCP[(uint8) busca].pid != id) && (busca < CANT_TAREAS) ){
		busca++;
	}
	return busca;
}


void cambiar_estado(uint16 bpcPos, uint8 estado_nuevo){

	BCP[bpcPos].estado = estado_nuevo;
	if(estado_nuevo == MATAR){
		BCP[BCP[bpcPos].ant].sig = BCP[bpcPos].sig;
		BCP[BCP[bpcPos].sig].ant = BCP[bpcPos].ant;
		cant_tareas_en_sistema--;
	}
}


uint16 cargarTarea(uint32 dir_tarea, uint32 tam, int8* name){



	// 1ro: copiar codigo de la tarea a una pagina nueva
	uint32* eip_fisico = pidoPagina();

	cpmem((uint8*) dir_tarea, (uint8*) eip_fisico, tam);

	// 2do: crear un directorio y las tablas de paginas necesarias y mapearlas segun corresponda, una pagina para la pila
	// y otra para el video
	uint32 *directorio = pidoPagina();
	uint32 *pila = pidoPagina();
	uint32 *pila0 = pidoPagina();
	uint16 *video = (uint16 *) pidoPagina();

	//mapeo las paginas que quiero con identity mapping
	mapeo_paginas_default(directorio);

	/*ESTO LO HACEMOS ASI ARBITRARIAMENTE. TODAS LAS TAREAS SE COMPILAN PONIENDO LA DIRECTIVA "ORG 0x2000".
	  POR ESTO MISMO, EL EIP SE PONE POR DEFECTO EN 0X2000*/
	mapear_pagina(directorio, 0x0, (uint32) eip_fisico, PRESENT | WRITE | USUARIO);
	/****************************************************************************************************/

	mapear_pagina(directorio, (uint32) pila, (uint32) pila, PRESENT | WRITE | USUARIO);
	mapear_pagina(directorio, (uint32) pila0, (uint32) pila0, PRESENT | WRITE | SUPERVISOR);
	//mapeo la pagina de video a la pagina de video de la tarea
	mapear_pagina(directorio, (uint32) 0xB8000, (uint32) video, PRESENT | WRITE | USUARIO);


/*	mover_puntero(5,0);*/
/*	printdword(obtener_mapeo(directorio, 0x2000), BASE16 | ROJO_L);*/
/*	breakpoint();*/

	// 3ro: crear una entrada de TSS e inicializarla
	uint8 pos_TSS = buscar_TSS_vacia();
	setmem((uint8*) &TSS[pos_TSS], 0x00, TAM_TSS);
	crear_TSS(pos_TSS, (uint32) directorio, 0x0, USER_EFLAGS, ((uint32)pila) + TAM_PAG, ((uint32)pila0) + TAM_PAG);


	// 4to: crear una entrada en la GDT para la TSS creada antes y mapearla
	uint16 pid = buscar_entradaGDT_vacia();
	gdt_vector[pid] = make_descriptor((uint32) &TSS[pos_TSS], TAM_TSS, TSS_AVAILABLE | PRESENTE | DPL_3 | TSS_0_OBLIGATORIO, TSS_GRANULARIDAD);


	// 5to: crear entrada de BCP e inicializarla
	crear_entradaBCP(pid, ACTIVO, directorio, video, name);

	return pid;
}



void matarTarea(uint8 id){
	uint8 bcpPos = buscar_entradaBCP(id);
	cambiar_estado(bcpPos, MATAR);
}



void mapeo_paginas_default(uint32* directorio){

	//pagina en donde empieza el codigo del kernel
	uint32 kernel_init = ((uint32) &make_descriptor) & 0xFFFFF000;

	//pagina siguiente a la pagina donde termina el kernel
	uint32 kernel_end = ( ((uint32) &command[99]) & 0xFFFFF000) + TAM_PAG;

	//mapeo la pagina en donde esta la estructura de la GDT
	mapear_pagina(directorio, (uint32) gdt_vector, (uint32) gdt_vector, PRESENT | READ_PAGINACION | USUARIO);

	//mapeo todas las paginas en donde se encuentra el kernel
	while(kernel_init != kernel_end){
		mapear_pagina(directorio, kernel_init, kernel_init, PRESENT | READ_PAGINACION | USUARIO);
		kernel_init += TAM_PAG;
	}

}



uint8 buscar_entradaBCP_matar(){

	uint8 res = 0;
	while( (res < CANT_TAREAS) && (BCP[res].estado != MATAR) ){
		res++;
	}

	return res; //NOTA: devuelve la posicion de la tarea en la BCP
}


void info_BCP(uint8 index){
	printf("\n",AZUL_L);
	printf("  pid: ", GRIS_L | BRILLANTE); printdword(BCP[index].pid, GRIS_L | BRILLANTE);
	//printf("  estado: ", GRIS_L | BRILLANTE); printdword(BCP[index].estado, GRIS_L | BRILLANTE);
	//printf("directorio: ", GRIS_L | BRILLANTE); printdword((uint32) BCP[index].entrada_directorio, BASE16 | GRIS_L | BRILLANTE); printf("\n",0);
	//printf("  siguiente: ", GRIS_L | BRILLANTE); printdword(BCP[index].sig, GRIS_L | BRILLANTE);
	//printf("anterior: ", GRIS_L | BRILLANTE); printdword(BCP[index].ant, GRIS_L | BRILLANTE); printf("\n",0);
	//printf("pantalla: ", GRIS_L | BRILLANTE); printdword((uint32) BCP[index].pantalla, BASE16 | GRIS_L | BRILLANTE); printf("\n",0);
}


void kill_app(uint16 bcpPos){

	uint8 running = FALSE;

	//solo mato a las tareas que estan corriendo o activas
	if( (BCP[bcpPos].estado == CORRIENDO) || (BCP[bcpPos].estado == ACTIVO)){

		if(BCP[bcpPos].estado == CORRIENDO){
			running = TRUE;
		}
		matarTarea(bcpPos);
		desaparecerTarea(bcpPos);

		info_BCP(bcpPos);
		breakpoint();

		if(running){
			switch_task();
		}
	}
}


void exit(){
	mover_puntero(3,3);
  printf("TERMINE!!",AZUL_L);
	matarTarea(BCP[tarea_actual].pid);
	switch_task();

}


void desaparecerTarea(uint8 bcpPos){

	//pongo en 0 y libero las paginas que se pidieron para el directorio y las tablas de pagina
	liberar_directorio(BCP[bcpPos].entrada_directorio);

	//recupero la direccion de la tss
	Tss* task_tss = (Tss*) ( ( ((uint32) gdt_vector[BCP[bcpPos].pid].base2) << 16 ) |\
				 ( (uint16) gdt_vector[BCP[bcpPos].pid].base1) );

	//recupero la dir de la pila
	uint8* task_pila = (uint8*) (task_tss->ebp & 0xFFFFF000);

	//pongo en 0 y libero pagina de la pila
	setmem(task_pila,0x00,TAM_PAG);
	liberoPagina((uint32*) task_pila);

	//recupero la pila0 de la tarea
	uint8* task_pila0 = (uint8*) (task_tss->esp0 & 0xFFFFF000);

	//pongo en 0 y libero pagina de la pila
	setmem(task_pila0,0x00,TAM_PAG);
	liberoPagina((uint32*) task_pila0);

	//pongo en 0 y libero pagina del video
	setmem((uint8*) BCP[bcpPos].pantalla,0x00,80*24*2);
	liberoPagina((uint32*) BCP[bcpPos].pantalla);

	//libero la tss (OJO: solo pone en 0 el cr3, pero por ahora es suficiente)
	vaciar_TSS(task_tss);

	//libero la entrada de GDT
	gdt_vector[BCP[bcpPos].pid] = make_descriptor(0,0,0,0);

	//libero la entrada del BCP
	BCP[bcpPos].estado = MUERTO;
}

