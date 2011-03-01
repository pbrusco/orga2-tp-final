#include "pantalla.h"
#include "../memoria/memoria.h"
#include "../bcp/bcp.h"
#include "../paginacion/paginacion.h"

extern BCP_Entry BCP[];
// VARIABLES GLOBALES

//TODO: esto deberia moverse a la ultima fila
word *puntero_pantalla = (word*) DIR_INI_PANTALLA;

//esta variable contiene el indice de la BCP de la tarea que se está viendo por pantalla
byte tarea_en_pantalla, tarea_a_mostrar;

//TODO: esto debería desaparecer
void avanzar_puntero(){
	puntero_pantalla++;
	if( (dword) puntero_pantalla >= DIR_FIN_PANTALLA )
		puntero_pantalla = (word *) DIR_INI_PANTALLA;
}

void retroceder_puntero(){
	puntero_pantalla--;
	if( (dword) puntero_pantalla < DIR_INI_PANTALLA )
		puntero_pantalla = (word *) DIR_FIN_PANTALLA;
}

void mover_puntero(byte fila, byte columna){
	puntero_pantalla = (word *) (DIR_INI_PANTALLA + (80*2*fila) + (columna*2));
}

void clear_screen(){
	puntero_pantalla = (word *) DIR_INI_PANTALLA;
	while( (dword) puntero_pantalla != DIR_FIN_PANTALLA){
		*puntero_pantalla = 0x0000;
		puntero_pantalla++;
	}
	puntero_pantalla = (word *) DIR_INI_PANTALLA;
}


//TODO: esto que es?
void fill_random_screen(){
	puntero_pantalla = (word *) DIR_INI_PANTALLA;
	while( (dword) puntero_pantalla != DIR_FIN_PANTALLA){
		*puntero_pantalla = 0xE200;
		puntero_pantalla++;
	}
	puntero_pantalla = (word *) DIR_INI_PANTALLA;

}

void salto_de_linea(){
	byte fila_actual = (((dword) puntero_pantalla) % DIR_INI_PANTALLA) / 160;
	puntero_pantalla = (word*) (DIR_INI_PANTALLA + 160*(fila_actual+1));
	if( ((dword) puntero_pantalla) >= DIR_FIN_PANTALLA){
		puntero_pantalla = (word *) DIR_INI_PANTALLA;
	}
		
}



/* TODO: por más que sea molesto, podriamos pasarle 2 parametros mas: fila y columna donde queremos que se imprima la "frase" */
void printf(const char *frase, const byte atrib){

	char c = *frase;

	while(c != '\0') {
		if(c != '\n')
			putc(c, atrib);
		else
			salto_de_linea();
		
		frase++;
		c = (byte) *frase;
	}
}

void printdword(const dword var, const word atr){
		
		//recupero la base
		byte base = (byte) (atr >> 8);
		
		//recupero los atributos de impresion
		byte atrib = (byte) (atr & 0x00FF);
		
		//aca guardo los digitos
		byte buffer[12];
		
		num2char(var, buffer, base);
		byte i=0;
		while(buffer[i] != '\0') {
		    putc(buffer[i], atrib);
		    i++;
		}
}




void printl(const char *frase, byte flag, byte atrib, dword param){

  puntero_pantalla = (word*) ( (int) puntero_pantalla - (((int)puntero_pantalla)%160) + 160);
	char c = *frase;

	while(c != '\0') {
		putc(c, atrib);
		frase++;
		c = (byte) *frase;
	}
	if(flag != 0){
		byte base = 10;
		if(flag == 2){
			base = 16;
		}
		//aca guardo los digitos
		byte buffer[12];
		num2char(param, buffer, base);
		byte i=0;
		while(buffer[i] != '\0') {
		    putc(buffer[i], atrib);
		    i++;
		}
	}

	frase++;
	c = *frase;
}



//TODO: si se elminina el avanzar_puntero, cambiar por *puntero++
void putc(char c, byte atrib){
	*puntero_pantalla = ( ((word) atrib) << 8 ) | ( (word) c );
	avanzar_puntero();
}


void borrarc(){
	retroceder_puntero();
  *puntero_pantalla = ( (word) ' ' );
}


void num2char(dword n, byte* buffer, dword base){

	byte digits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	word i = 0;

	//genera los digitos al reves
	do {
		buffer[i] = digits[n%base];
		i++;
		n = n/base;
	} while (n > 0);
	buffer[i] = '\0';
	i--;

	//reverse buffer
	word size = i + 1;
	word j=0;
	while(j < size/2) {
		byte tmp = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = tmp;
		j++;
		i--;
	}
}

char num2char2(const int n){

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

int char2num(char c){

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


void mostrar_pantalla_entera(){
	//limpio las interrupciones
	cli();

	//si tengo que cambiar la pantalla
	if(tarea_en_pantalla != tarea_a_mostrar){

		//copio la pantalla a donde debe escribir realmente la tarea_en_pantalla
		cpmem((byte*) DIR_INI_PANTALLA, (byte*) BCP[tarea_en_pantalla].pantalla, TAM_PANTALLA_TAREA);

		//copio la pagina de video de la "tarea_a_mostrar" a la pantalla
		cpmem((byte*) BCP[tarea_a_mostrar].pantalla, (byte*) DIR_INI_PANTALLA, TAM_PANTALLA_TAREA);

		//remapeo la pagina de video de la tarea a donde le corresponde escribir
		mapear_pagina(	BCP[tarea_en_pantalla].entrada_directorio,
				DIR_INI_PANTALLA,
				(dword) BCP[tarea_en_pantalla].pantalla,
				USUARIO | WRITE | PRESENT);

		//remapeo la pagina de video de la "tarea_a_mostrar" a la pantalla
		mapear_pagina(	BCP[tarea_a_mostrar].entrada_directorio,
				DIR_INI_PANTALLA,
				DIR_INI_PANTALLA,
				USUARIO | WRITE | PRESENT);

		//actualizo la variable "tarea_en_pantalla"
		tarea_en_pantalla = tarea_a_mostrar;
	}

	//activo las interrupciones nuevamente
	sti();
}

void cambiar_de_pantalla(byte bcpPos){
	tarea_a_mostrar = bcpPos;
}

