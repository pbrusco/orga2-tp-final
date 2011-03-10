#ifndef __IDT_H__
#define __IDT_H__

//Interrupt Descriptor Table - Definicion de las estructuras de la IDT.


//Struct de descriptor de IDT 
typedef struct str_idt_descriptor {
	unsigned short idt_length;
	unsigned int idt_addr;
} __attribute__((__packed__)) IDT_Descriptor;

// Struct de una entrada de la IDT
typedef struct str_idt_entry_fld {
		word offset_0_15;
		word segsel;
		byte ceros;
		word tipo : 5;
		word dpl : 2;
		word presente : 1;
		word offset_16_31;
} __attribute__((__packed__, aligned (8))) IDT_Entry;


/* Estos metodos sirven para completar las entradas del vector IDT para administrar los handlers de interrupciones */
void set_interrupt(IDT_Entry* idt, dword handler, word dpl, word segmento);
void set_trap(IDT_Entry* idt, dword handler, word dpl, word segmento);
void set_task_gate(IDT_Entry* idt, dword handler, word dpl, word segmento);


/* Metodo para inicializar los descriptores de la IDT */
void idtFill();//TODO: va a cambiar


/* Atencion de interrupciones */
void handler_de_interrupciones(word nro_int);

#endif //__IDT_H__
