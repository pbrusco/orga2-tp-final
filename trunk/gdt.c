#include "gdt.h"
#include "tss.h"


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

gdt_entry gdt[GDT_COUNT]; 
// Desc. de segmento nulo
gdt[0] = (gdt_entry) make_descriptor( 0, 0, 0, 0);
// Desc. de segmento de codigo
gdt[1] = (gdt_entry) make_descriptor( 0, 0xfffff, PRESENTE | DPL_0 | GENERAL | CODIGO, GRANULARIDAD | DB); 
// Descriptor de segmento de datos
gdt[2] = (gdt_entry) make_descriptor( 0, 0xfffff, PRESENTE | DPL_0 | GENERAL | DATA | WRITE, GRANULARIDAD | DB);
// Descriptor segmento de memoria de video
gdt[3] = (gdt_entry) make_descriptor(0x000B8000, 0x00F9F, PRESENTE | DPL_0 | GENERAL | DATA | WRITE, GRANULARIDAD | DB);
// Descriptor de task 0
gdt[4] = (gdt_entry) make_descriptor(0x000000, 0x0000067, PRESENTE | DPL_0 | SISTEMA | TSS_AVAILABLE, GRANULARIDAD);
// Descriptor de task 1
gdt[5] = (gdt_entry) make_descriptor(0x000000, 0x0000067, PRESENTE | DPL_0 | SISTEMA | TSS_AVAILABLE, GRANULARIDAD);
// Descriptor de task 2
gdt[6] = (gdt_entry) make_descriptor(0x000000, 0x0000067, PRESENTE | DPL_0 | SISTEMA | TSS_AVAILABLE, GRANULARIDAD); 


// Definicion del GDTR
gdt_desc GDT_DESC = {sizeof(gdt)-1, (unsigned int)&gdt};


