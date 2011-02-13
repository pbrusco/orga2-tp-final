#include "pantalla.h"

// VARIABLES GLOBALES
word *puntero_pantalla = DIR_INI_PANTALLA;

void avanzar_puntero(){
	puntero_pantalla++;
	if( (dword) puntero_pantalla >= DIR_FIN_PANTALLA ) 
		puntero_pantalla = (word *) DIR_INI_PANTALLA;
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


void printf(const char *frase, byte flag, byte atrib, dword param){

	byte c = (byte) *frase;
    
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
	c = (byte) *frase;
}

void putc(byte c, byte atrib){
	*puntero_pantalla = ( ((word) atrib) << 8 ) | ( (word) c );
	avanzar_puntero();
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
