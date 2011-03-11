#include "../tipos/tipos_basicos.h"
#include "gdt.h"

extern gdt_entry gdt_vector[];

//funcion que crea una entrada para la gdt
gdt_entry make_descriptor(uint32 base, uint32 limite, uint8 atr1, uint8 atr2 ) {
	gdt_entry entrada;
	entrada.base1 = (uint16) (base & 0Xffff);
	entrada.base2 = (uint8) ((base & 0xff0000) >> 16);
	entrada.base3 = (uint8) ((base & 0xff000000) >> 24);
	entrada.limite1 = (uint16) (limite & 0xffff);
	entrada.atr2    = (uint8) (atr2 | ((limite & 0xf0000) >> 16));
	entrada.atr1    = atr1;
	return entrada;
}


//funciones para modificar la GDT

//devuelve una posicion de la gdt que este vacia
uint16 buscar_entradaGDT_vacia(){
	int32 i = 1;

	while(i < GDT_COUNT){
		if ( gdt_vector[i].limite1 == 0 ) return i;
		i++;
	}
	return 0;
}

void borrar_gdt_entry(uint16 i){
	gdt_vector[i] = make_descriptor(0,0,0,0);
}

