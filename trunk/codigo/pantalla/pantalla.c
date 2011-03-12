#include "../tipos/tipos_basicos.h"
#include "pantalla.h"
#include "../bcp/bcp.h"
#include "../paginacion/paginacion.h"
#include "../memoria/memoria.h"

extern BCP_Entry BCP[];


/********************************************************************\
****************VARIABLES Y FUNCIONES DE DEBUGGING*******************\
*********************************************************************/
uint16 *puntero_pantalla = (uint16*) DIR_INI_PANTALLA;


void avanzar_puntero(){
	puntero_pantalla++;
	if( (uint32) puntero_pantalla >= DIR_FIN_PANTALLA )
		puntero_pantalla = (uint16 *) DIR_INI_PANTALLA;
}

void retroceder_puntero(){
	puntero_pantalla--;
	if( (uint32) puntero_pantalla < DIR_INI_PANTALLA )
		puntero_pantalla = (uint16 *) DIR_FIN_PANTALLA;
}

void mover_puntero(uint8 fila, uint8 columna){
	puntero_pantalla = (uint16 *) (DIR_INI_PANTALLA + (80*2*fila) + (columna*2));
}

void salto_de_linea(){
	uint8 fila_actual = (((uint32) puntero_pantalla) % DIR_INI_PANTALLA) / 160;
	puntero_pantalla = (uint16*) (DIR_INI_PANTALLA + 160*(fila_actual+1));
	if( ((uint32) puntero_pantalla) >= DIR_FIN_PANTALLA){
		puntero_pantalla = (uint16 *) DIR_INI_PANTALLA;
	}

}

void printf(const int8 *frase, const uint8 atrib){

	int8 c = *frase;

	while(c != '\0') {
		if(c == '\n'){
			salto_de_linea();
		}
		else if(c == '\t'){
			putc(' ', atrib);
			putc(' ', atrib);
			putc(' ', atrib);
			putc(' ', atrib);
		}
		else{
			putc(c, atrib);
		}
		frase++;
		c = (uint8) *frase;
	}
}

void printdword(const uint32 var, const uint16 atr){

		//recupero la base
		uint8 base = (uint8) (atr >> 8);

		//si la base es cero, pongo por default base 10
		if(base == 0){
			base = 10;
		}

		//recupero los atributos de impresion
		uint8 atrib = (uint8) (atr & 0x00FF);

		//aca guardo los digitos
		uint8 buffer[12];

		num2char(var, buffer, base);
		uint8 i=0;
		while(buffer[i] != '\0') {
		 	putc(buffer[i], atrib);
			i++;
		}
}

void putc(int8 c, uint8 atrib){
	*puntero_pantalla = ( ((uint16) atrib) << 8 ) | ( (uint16) c );
	avanzar_puntero();
}


void borrarc(){
	retroceder_puntero();
	*puntero_pantalla = ( (uint16) ' ' );
}


void num2char(uint32 n, uint8* buffer, uint32 base){

	uint8 digits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	uint16 i = 0;

	//genera los digitos al reves
	do {
		buffer[i] = digits[n%base];
		i++;
		n = n/base;
	} while (n > 0);
	buffer[i] = '\0';
	i--;

	//reverse buffer
	uint16 size = i + 1;
	uint16 j=0;
	while(j < size/2) {
		uint8 tmp = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = tmp;
		j++;
		i--;
	}
}

void clear_screen(){
	puntero_pantalla = (uint16 *) DIR_INI_PANTALLA;
	while( (uint32) puntero_pantalla != DIR_FIN_PANTALLA){
		*puntero_pantalla = 0x0000;
		puntero_pantalla++;
	}
	puntero_pantalla = (uint16 *) DIR_INI_PANTALLA;
}


/********************************************************************\
***********VARIABLES Y FUNCIONES PARA MANEJO DE PANTALLAS************\
*********************************************************************/

//esta variable contiene el indice de la BCP de la tarea que se está viendo por pantalla
uint8 tarea_en_pantalla, tarea_a_mostrar;

//este es el puntero que se va a usar para la consola
uint16* cursor_consola = (uint16*) (DIR_INI_PANTALLA + 80*2*24 + 4);

//este puntero se va a usar para mostrar informacion
uint16* cursor_informacion = (uint16*) DIR_INI_PANTALLA;

int8* prompt = "$>";


void cambiar_pantalla(uint8 pid){
  tarea_a_mostrar = pid;
  mostrar_pantalla_entera();
}

void mostrar_pantalla_entera(){

  uint8 bcp_tarea_en_pantalla = buscar_entradaBCP(tarea_en_pantalla);
  uint8 bcp_tarea_a_mostrar = buscar_entradaBCP(tarea_a_mostrar);

		//si la tarea en pantalla no esta muerta
	if(BCP[bcp_tarea_en_pantalla].estado != MUERTO){
		//copio la pantalla a donde debe escribir realmente la tarea_en_pantalla
		cpmem((uint8*) ( DIR_INI_PANTALLA + (80*2) ),
		 		(uint8*) ( ((uint32) BCP[bcp_tarea_en_pantalla].pantalla) + (80*2)),
		 		TAM_PANTALLA_TAREA);
  }

	//copio la pagina de video de la "tarea_a_mostrar" a la pantalla
	cpmem(	(uint8*) (((uint32) BCP[bcp_tarea_a_mostrar].pantalla) + 80*2),
		(uint8*) (DIR_INI_PANTALLA + (80*2)),
		TAM_PANTALLA_TAREA);


	//si la tarea en pantalla NO es el KERNEL
	if(tarea_en_pantalla != 5){
		//remapeo la pagina de video de la tarea a donde le corresponde escribir
		mapear_pagina(	BCP[bcp_tarea_en_pantalla].entrada_directorio,
				DIR_INI_PANTALLA,
				(uint32) BCP[bcp_tarea_en_pantalla].pantalla,
				USUARIO | WRITE | PRESENT);
	}

	//remapeo la pagina de video de la "tarea_a_mostrar" a la pantalla
	mapear_pagina(	BCP[bcp_tarea_a_mostrar].entrada_directorio,
			DIR_INI_PANTALLA,
			DIR_INI_PANTALLA,
			USUARIO | WRITE | PRESENT);

	//actualizo la variable "tarea_en_pantalla"
	tarea_en_pantalla = tarea_a_mostrar;

}

void clear_command_line(){
	setmem((uint8*) DIR_INI_PANTALLA + 80*2*24, 0x00, 80*2);
	cursor_consola = (uint16 *) DIR_INI_COMMAND;
	mover_puntero(24,0);
	printf(prompt, COLOR_PROMPT);
}


void clear_info_line(){
	setmem((uint8*) DIR_INI_PANTALLA, 0x00, 80*2);
	cursor_informacion = (uint16*) DIR_INI_PANTALLA;
}


void removerc(){
	//si no esta el cursor al lado del prompt
	if( ((uint32)cursor_consola) != DIR_INI_COMMAND ){
		cursor_consola--;
		*cursor_consola = 0x0000;
	}
}


void agregarc(uint8 letra, uint8 atrib){
	//si NO se llenó linea en la que estoy intentando escribir
	if(   ( ((uint32) cursor_consola) != (DIR_FIN_PANTALLA) )   ){
		*cursor_consola = ( ((uint16) atrib) << 8 ) | ( (uint16) letra );
		cursor_consola++;
	}
}


int8 num2char2(const int32 n){

switch(n){
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    default: //si hay un error
      return 'I';
    }

}

int32 char2num(int8 c){

  switch(c){
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    }
    return -1;

}

