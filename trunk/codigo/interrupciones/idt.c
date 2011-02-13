#include "isr.h"
#include "idt.h"


//Metodo para inicializar los descriptores de la IDT

void idtFill() {

	idt[14].offset_0_15 = (unsigned short) ((unsigned int)(&_isrE) & (unsigned int) 0xFFFF); \
	idt[14].segsel = (unsigned short) 0x0008; \
	idt[14].attr = (unsigned short) 0x8E00; \
	idt[14].offset_16_31 = (unsigned short) ((unsigned int)(&_isrE) >> 16 & (unsigned int) 0xFFFF);

	idt[32].offset_0_15 = (unsigned short) ((unsigned int)(&_isr20) & (unsigned int) 0xFFFF); \
	idt[32].segsel = (unsigned short) 0x0008; \
	idt[32].attr = (unsigned short) 0x8E00; \
	idt[32].offset_16_31 = (unsigned short) ((unsigned int)(&_isr20) >> 16 & (unsigned int) 0xFFFF);


	idt[33].offset_0_15 = (unsigned short) ((unsigned int)(&_isr21) & (unsigned int) 0xFFFF); \
	idt[33].segsel = (unsigned short) 0x0008; \
	idt[33].attr = (unsigned short) 0x8E00; \
	idt[33].offset_16_31 = (unsigned short) ((unsigned int)(&_isr21) >> 16 & (unsigned int) 0xFFFF);
}

//IDT
idt_entry idt[256] = {};

//Descriptor de la IDT (para cargar en IDTR)
idt_descriptor IDT_DESC = {sizeof(idt)-1, (unsigned int)&idt};

