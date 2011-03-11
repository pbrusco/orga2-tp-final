#ifndef __IDT_H__
#define __IDT_H__

//Interrupt Descriptor Table - Definicion de las estructuras de la IDT.


//Struct de descriptor de IDT 
typedef struct str_idt_descriptor {
	uint16 idt_length;
	uint32 idt_addr;
} __attribute__((__packed__)) IDT_Descriptor;

// Struct de una entrada de la IDT
typedef struct str_idt_entry_fld {
		uint16 offset_0_15;
		uint16 segsel;
		uint8 ceros;
		uint16 tipo : 5;
		uint16 dpl : 2;
		uint16 presente : 1;
		uint16 offset_16_31;
} __attribute__((__packed__, aligned (8))) IDT_Entry;


/* Estos metodos sirven para completar las entradas del vector IDT para administrar los handlers de interrupciones */
void set_interrupt(IDT_Entry* idt, uint32 handler, uint16 dpl, uint16 segmento);
void set_trap(IDT_Entry* idt, uint32 handler, uint16 dpl, uint16 segmento);
void set_task_gate(IDT_Entry* idt, uint32 handler, uint16 dpl, uint16 segmento);


/* Metodo para inicializar los descriptores de la IDT */
void idtFill();//TODO: va a cambiar


/* Atencion de interrupciones */
void handler_de_interrupciones(uint16 nro_int);

#endif //__IDT_H__
