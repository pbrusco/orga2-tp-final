#ifndef __TECLADO_H__
#define __TECLADO_H__

#define TECLADO_LONG


typedef struct _tecla{

	char letra;
	short codigo;
	//el codigo de break es +0x80 el codigo de la tecla
}__attribute__((__packed__)) tecla;


extern tecla teclado[];/* = {

	(tecla)	{ (short) 'a', (short) 0x1E } ,
	(tecla)	{ (short) 'b', (short) 0x30 } ,
	(tecla)	{ (short) 'c', (short) 0x2E } ,
	(tecla)	{ (short) 'd', (short) 0x20 } ,
	(tecla)	{ (short) 'e', (short) 0x12 } ,
	(tecla)	{ (short) 'f', (short) 0x21 } ,
	(tecla)	{ (short) 'g', (short) 0x22 } ,
	(tecla)	{ (short) 'h', (short) 0x23 } ,
	(tecla)	{ (short) 'i', (short) 0x17 } ,
	(tecla)	{ (short) 'j', (short) 0x24 } ,
	(tecla)	{ (short) 'k', (short) 0x25 } ,
	(tecla)	{ (short) 'l', (short) 0x26 } ,
	(tecla)	{ (short) 'm', (short) 0x32 } ,
	(tecla)	{ (short) 'n', (short) 0x31 } ,
	(tecla)	{ (short) 'o', (short) 0x18 } ,
	(tecla)	{ (short) 'p', (short) 0x19 } ,
	(tecla)	{ (short) 'q', (short) 0x10 } ,
	(tecla)	{ (short) 'r', (short) 0x13 } ,
	(tecla)	{ (short) 's', (short) 0x1F } ,
	(tecla)	{ (short) 't', (short) 0x14 } ,
	(tecla)	{ (short) 'u', (short) 0x16 } ,
	(tecla)	{ (short) 'v', (short) 0x2F } ,
	(tecla)	{ (short) 'w', (short) 0x11 } ,
	(tecla)	{ (short) 'x', (short) 0x2D } ,
	(tecla)	{ (short) 'y', (short) 0x15 } ,
	(tecla)	{ (short) '1', (short) 0x02 } ,
	(tecla)	{ (short) '2', (short) 0x03 } ,
	(tecla)	{ (short) '3', (short) 0x04 } ,
	(tecla)	{ (short) '4', (short) 0x05 } ,
	(tecla)	{ (short) '5', (short) 0x06 } ,
	(tecla)	{ (short) '6', (short) 0x07 } ,
	(tecla)	{ (short) '7', (short) 0x08 } ,
	(tecla)	{ (short) '8', (short) 0x09 } ,
	(tecla)	{ (short) '9', (short) 0x0A } ,
	(tecla)	{ (short) '0', (short) 0x0B } ,
	(tecla)	{ (short) '-', (short) 0x0C } ,
	(tecla)	{ (short) ',', (short) 0x33 } ,
	(tecla)	{ (short) '.', (short) 0x34 }

};*/


char GetChar(short codigo);
short GetPos(short codigo);
void tecladoFill();




#endif
