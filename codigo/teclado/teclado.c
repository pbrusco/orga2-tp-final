#include "../tipos/tipos_basicos.h"
#include "teclado.h"
#include "../tipos/tipos_basicos.h"
#include "../pantalla/pantalla.h"


int8 teclado[] = "! 1234567890- < "	//0x00 a 0x0f (falta el 0x01, 0x0d y 0x0f)
		  "qwertyuiop  * as"	//0x10 a 0x1f (falta el 0x1a, 0x1b y 0x1d)
		  "dfghjkl     zxcv"	//0x20 a 0x2f (falta el 0x27, 0x28, 0x29, 0x2a y 0x2b)
		  "bnm,.           "	//0x30 a 0x3f (faltan del 0x35 al 0x3f menos el 0x39)
		  "                "	//DE ACA EN ADELANTE FALTAN TODOS
		  "                "
		  "                "
		  "                ";	//tiene tamaño 127

int8 getChar(int16 code){
	if (code > 127) {return '!';}
	else{return teclado[code];}
}

