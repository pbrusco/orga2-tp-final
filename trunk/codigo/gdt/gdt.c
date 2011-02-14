#include "../tipos/tipos_basicos.h"
#include "gdt.h"

extern gdt_entry gdt_vector[];

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


//funciones para modificar la GDT

//devuelve una posicion de la gdt que este vacia
word buscar_entradaGDT_vacia(){
	int i = 1;

	while(i < GDT_COUNT){
		if ( gdt_vector[i].limite1 == 0 ) return i;
		i++;
	}
	return 0;
}

void borrar_gdt_entry(word i){
	gdt_vector[i] = make_descriptor(0,0,0,0);
}

