#include "../tipos/tipos_basicos.h"
#include "gdt.h"

//funcion que crea una entrada para la gdt
gdt_entry make_descriptor(dword base, dword limite, byte atr1, byte atr2 ) {
	gdt_entry entrada;
	entrada.base1 = (word) (base & 0Xffff);
	entrada.base2 = (byte) ((base & 0xff0000) >> 16);
	entrada.base3 = (byte) ((base & 0xff000000) >> 24);
	entrada.limite1 = (word) (limite & 0xffff);
	entrada.atr2    = (byte) (atr2 | ((limite & 0xf0000) >> 16));
	entrada.atr1    = atr1;
	return entrada;
}


//Definicion de la GDT
gdt_entry gdt[GDT_COUNT] = {

	/* Descriptor de segmento nulo*/	
	(gdt_entry){(unsigned long long) 0},

	
	/* Descriptor de segmento de codigo KERNEL*/
	(gdt_entry){ 
		(word) 0xFFFF,				//limit 0:15 	(16 bits)
		(word) 0x0000,				//base 0:15		(16 bits)
		(byte) 0x00, 				//base 23:16	(7 bits)
		(byte) (PRESENTE | DPL_0 | GENERAL | CODIGO),
		(byte) (GRANULARIDAD | DB | 0xF),
		(byte) 0x00 				//base_31_24	(16 bits)
	},
	
	/* Descriptor de segmento de datos KERNEL*/
	(gdt_entry){ 
		(word) 0xFFFF,				//limit 0:15 	(16 bits)
		(word) 0x0000,				//base 0:15		(16 bits)
		(byte) 0x00, 				//base 23:16	(7 bits)
		(byte) (PRESENTE | DPL_0 | GENERAL | DATOS),
		(byte) (GRANULARIDAD | DB | 0xF),
		(byte) 0x00 				//base_31_24	(16 bits)
	},
	
	/* Descriptor de segmento de codigo USER*/
	(gdt_entry){ 
		(word) 0xFFFF,				//limit 0:15 	(16 bits)
		(word) 0x0000,				//base 0:15		(16 bits)
		(byte) 0x00, 				//base 23:16	(7 bits)
		(byte) (PRESENTE | DPL_3 | GENERAL | CODIGO),
		(byte) (GRANULARIDAD | DB | 0xF),
		(byte) 0x00 				//base_31_24	(16 bits)
	},
	
	/* Descriptor de segmento de datos USER*/
	(gdt_entry){ 
		(word) 0xFFFF,				//limit 0:15 	(16 bits)
		(word) 0x0000,				//base 0:15		(16 bits)
		(byte) 0x00, 				//base 23:16	(7 bits)
		(byte) (PRESENTE | DPL_3 | GENERAL | DATOS),
		(byte) (GRANULARIDAD | DB | 0xF),
		(byte) 0x00 				//base_31_24	(16 bits)
	},

};



// Definicion del GDTR
gdt_desc GDT_DESC = {sizeof(gdt)-1, (unsigned int)&gdt};





//funciones para modificar la GDT

//devuelve una posicion de la gdt que este vacia
word buscar_entradaGDT_vacia(){
	int i = 1;

	while(i < GDT_COUNT){
		if ( gdt[i].limite1 == 0 ) return i;
		i++;
	}
	return 0;
}

void borrar_gdt_entry(word i){
	gdt[i] = make_descriptor(0,0,0,0);
}

