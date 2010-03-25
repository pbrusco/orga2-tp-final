#include "teclado.h"

void tecladoFill(){

	teclado[0] = (tecla)	{ (char) 'a', (short) 0x1E }; //[teclado] = 'a'...[teclado+1] = 0x1e...[teclado+2] = 0x00...
	teclado[1] = (tecla)	{ (char) 'b', (short) 0x30 };
	teclado[2] = (tecla)	{ (char) 'c', (short) 0x2E };
	teclado[3] = (tecla)	{ (char) 'd', (short) 0x20 };
	teclado[4] = (tecla)	{ (char) 'e', (short) 0x12 };
	teclado[5] = (tecla)	{ (char) 'f', (short) 0x21 };
	teclado[6] = (tecla)	{ (char) 'g', (short) 0x22 };
	teclado[7] = (tecla)	{ (char) 'h', (short) 0x23 };
	teclado[8] = (tecla)	{ (char) 'i', (short) 0x17 };
	teclado[9] = (tecla)	{ (char) 'j', (short) 0x24 };
	teclado[10] = (tecla)	{ (char) 'k', (short) 0x25 };
	teclado[11] = (tecla)	{ (char) 'l', (short) 0x26 };
	teclado[12] = (tecla)	{ (char) 'm', (short) 0x32 };
	teclado[13] = (tecla)	{ (char) 'n', (short) 0x31 };
	teclado[14] = (tecla)	{ (char) 'o', (short) 0x18 };
	teclado[15] = (tecla)	{ (char) 'p', (short) 0x19 };
	teclado[16] = (tecla)	{ (char) 'q', (short) 0x10 };
	teclado[17] = (tecla)	{ (char) 'r', (short) 0x13 };
	teclado[18] = (tecla)	{ (char) 's', (short) 0x1F };
	teclado[19] = (tecla)	{ (char) 't', (short) 0x14 };
	teclado[20] = (tecla)	{ (char) 'u', (short) 0x16 };
	teclado[21] = (tecla)	{ (char) 'v', (short) 0x2F };
	teclado[22] = (tecla)	{ (char) 'w', (short) 0x11 };
	teclado[23] = (tecla)	{ (char) 'x', (short) 0x2D };
	teclado[24] = (tecla)	{ (char) 'y', (short) 0x15 };
	teclado[25] = (tecla)	{ (char) '1', (short) 0x02 };
	teclado[26] = (tecla)	{ (char) '2', (short) 0x03 };
	teclado[27] = (tecla)	{ (char) '3', (short) 0x04 };
	teclado[28] = (tecla)	{ (char) '4', (short) 0x05 };
	teclado[29] = (tecla)	{ (char) '5', (short) 0x06 };
	teclado[30] = (tecla)	{ (char) '6', (short) 0x07 };
	teclado[31] = (tecla)	{ (char) '7', (short) 0x08 };
	teclado[32] = (tecla)	{ (char) '8', (short) 0x09 };
	teclado[33] = (tecla)	{ (char) '9', (short) 0x0A };
	teclado[34] = (tecla)	{ (char) '0', (short) 0x0B };
	teclado[35] = (tecla)	{ (char) '-', (short) 0x0C };
	teclado[36] = (tecla)	{ (char) ',', (short) 0x33 };
	teclado[37] = (tecla)	{ (char) '.', (short) 0x34 };
	teclado[38] = (tecla)	{ (char) 'z', (short) 0x2C };
	

}

tecla teclado[39];

char GetChar(short code){

	short pos = GetPos(code);
	
	return teclado[pos].letra;
}

short GetPos(short code){

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
