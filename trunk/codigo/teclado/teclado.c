#include "teclado.h"

tecla teclado[39] = {

	(tecla)	{ (char) 'a', (short) 0x1E }, //[teclado] = 'a'...[teclado+1] = 0x1e...[teclado+2] = 0x00...
	(tecla)	{ (char) 'b', (short) 0x30 },
	(tecla)	{ (char) 'c', (short) 0x2E },
	(tecla)	{ (char) 'd', (short) 0x20 },
	(tecla)	{ (char) 'e', (short) 0x12 },
	(tecla)	{ (char) 'f', (short) 0x21 },
	(tecla)	{ (char) 'g', (short) 0x22 },
	(tecla)	{ (char) 'h', (short) 0x23 },
	(tecla)	{ (char) 'i', (short) 0x17 },
	(tecla)	{ (char) 'j', (short) 0x24 },
	(tecla)	{ (char) 'k', (short) 0x25 },
	(tecla)	{ (char) 'l', (short) 0x26 },
	(tecla)	{ (char) 'm', (short) 0x32 },
	(tecla)	{ (char) 'n', (short) 0x31 },
	(tecla)	{ (char) 'o', (short) 0x18 },
	(tecla)	{ (char) 'p', (short) 0x19 },
	(tecla)	{ (char) 'q', (short) 0x10 },
	(tecla)	{ (char) 'r', (short) 0x13 },
	(tecla)	{ (char) 's', (short) 0x1F },
	(tecla)	{ (char) 't', (short) 0x14 },
	(tecla)	{ (char) 'u', (short) 0x16 },
	(tecla)	{ (char) 'v', (short) 0x2F },
	(tecla)	{ (char) 'w', (short) 0x11 },
	(tecla)	{ (char) 'x', (short) 0x2D },
	(tecla)	{ (char) 'y', (short) 0x15 },
	(tecla)	{ (char) '1', (short) 0x02 },
	(tecla)	{ (char) '2', (short) 0x03 },
	(tecla)	{ (char) '3', (short) 0x04 },
	(tecla)	{ (char) '4', (short) 0x05 },
	(tecla)	{ (char) '5', (short) 0x06 },
	(tecla)	{ (char) '6', (short) 0x07 },
	(tecla)	{ (char) '7', (short) 0x08 },
	(tecla)	{ (char) '8', (short) 0x09 },
	(tecla)	{ (char) '9', (short) 0x0A },
	(tecla)	{ (char) '0', (short) 0x0B },
	(tecla)	{ (char) '-', (short) 0x0C },
	(tecla)	{ (char) ',', (short) 0x33 },
	(tecla)	{ (char) '.', (short) 0x34 },
	(tecla)	{ (char) 'z', (short) 0x2C }
};

char getChar(short code){

	short pos = getPos(code);
	
	return teclado[pos].letra;
}

short getPos(short code){

	short res = 0;
	short i;
	for(i = 0; i < 39 ; i++){
		if(teclado[i].codigo == code){
			res = i;
			i = 39 ;
		}
	}
	return res;
}
