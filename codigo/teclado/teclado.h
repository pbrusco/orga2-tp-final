#ifndef __TECLADO_H__
#define __TECLADO_H__

#define TECLADO_LONG


typedef struct _tecla{

	char letra;
	short codigo;
	//el codigo de break es +0x80 el codigo de la tecla
}__attribute__((__packed__)) tecla;


char getChar(short codigo);
short getPos(short codigo);
void tecladoFill();


#endif

