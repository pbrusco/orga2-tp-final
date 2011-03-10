#include "../tipos/tipos_basicos.h"
#include "isr.h"
#include "idt.h"
#include "../pantalla/pantalla.h"

/*********************************************************************************\
\****** DECLARO LAS ESTRUCTURAS NECESARIAS PARA ATENDER LAS INTERRUPCIONES *******\
\*********************************************************************************/

IDT_Entry IDT[256] = {};

IDT_Descriptor IDT_DESC = {sizeof(IDT)-1, (unsigned int)&IDT};


void set_interrupt(IDT_Entry* idt, dword handler, word dpl, word segmento){
	idt->offset_0_15 = (word) (handler & 0x0000FFFF);
	idt->offset_16_31 = (word) (handler >> 16);
	idt->segsel = segmento;
	idt->ceros = 0x00;
	idt->tipo = 0x0E;
	idt->dpl = dpl;
	idt->presente = 1;	
}

void set_trap(IDT_Entry* idt, dword handler, word dpl, word segmento){
	idt->offset_0_15 = (word) (handler & 0x0000FFFF);
	idt->offset_16_31 = (word) (handler >> 16);
	idt->segsel = segmento;
	idt->ceros = 0x00;
	idt->tipo = 0x0F;
	idt->dpl = dpl;
	idt->presente = 1;	
}

void set_task_gate(IDT_Entry* idt, dword handler, word dpl, word segmento){
	idt->offset_0_15 = 0;
	idt->offset_16_31 = 0;
	idt->segsel = segmento;
	idt->ceros = 0x00;
	idt->tipo = 0x05;
	idt->dpl = dpl;
	idt->presente = 1;	
}



void idtFill() {

/*	idt[19].offset_0_15 = (unsigned short) ((unsigned int)(&_isr13) & (unsigned int) 0xFFFF); \*/
/*	idt[19].segsel = (unsigned short) 0x0008; \*/
/*	idt[19].attr = (unsigned short) 0x8E00; \*/
/*	idt[19].offset_16_31 = (unsigned short) ((unsigned int)(&_isr13) >> 16 & (unsigned int) 0xFFFF);*/


/*	idt[32].offset_0_15 = (unsigned short) ((unsigned int)(&_isr20) & (unsigned int) 0xFFFF); \*/
/*	idt[32].segsel = (unsigned short) 0x0008; \*/
/*	idt[32].attr = (unsigned short) 0x8E00; \*/
/*	idt[32].offset_16_31 = (unsigned short) ((unsigned int)(&_isr20) >> 16 & (unsigned int) 0xFFFF);*/


/*	idt[33].offset_0_15 = (unsigned short) ((unsigned int)(&_isr21) & (unsigned int) 0xFFFF); \*/
/*	idt[33].segsel = (unsigned short) 0x0008; \*/
/*	idt[33].attr = (unsigned short) 0x8E00; \*/
/*	idt[33].offset_16_31 = (unsigned short) ((unsigned int)(&_isr21) >> 16 & (unsigned int) 0xFFFF);*/
/*	*/
/*	*/
/*	idt[0x80].offset_0_15 = (unsigned short) ((unsigned int)(&_isr80) & (unsigned int) 0xFFFF); \*/
/*	idt[0x80].segsel = (unsigned short) 0x0008; \*/
/*	idt[0x80].attr = (unsigned short) 0xEE00; \*/
/*	idt[0x80].offset_16_31 = (unsigned short) ((unsigned int)(&_isr80) >> 16 & (unsigned int) 0xFFFF);*/
	
	/* Las primeras 20 interrupciones son las default. */
	set_interrupt(IDT + 0, (dword) &_INT0, 0, KERNEL_CS);
	set_interrupt(IDT + 1, (dword) &_INT1, 0, KERNEL_CS);
	set_interrupt(IDT + 2, (dword) &_INT2, 0, KERNEL_CS);
	set_interrupt(IDT + 3, (dword) &_INT3, 0, KERNEL_CS);
	set_interrupt(IDT + 4, (dword) &_INT4, 0, KERNEL_CS);
	set_interrupt(IDT + 5, (dword) &_INT5, 0, KERNEL_CS);
	set_interrupt(IDT + 6, (dword) &_INT6, 0, KERNEL_CS);
	set_interrupt(IDT + 7, (dword) &_INT7, 0, KERNEL_CS);
	set_interrupt(IDT + 8, (dword) &_INT8, 0, KERNEL_CS);
	set_interrupt(IDT + 9, (dword) &_INT9, 0, KERNEL_CS);
	set_interrupt(IDT + 10, (dword) &_INT10, 0, KERNEL_CS);
	set_interrupt(IDT + 11, (dword) &_INT11, 0, KERNEL_CS);
	set_interrupt(IDT + 12, (dword) &_INT12, 0, KERNEL_CS);
	set_interrupt(IDT + 13, (dword) &_INT13, 0, KERNEL_CS);
	set_interrupt(IDT + 14, (dword) &_INT14, 0, KERNEL_CS);
	set_interrupt(IDT + 15, (dword) &_INT15, 0, KERNEL_CS);
	set_interrupt(IDT + 16, (dword) &_INT16, 0, KERNEL_CS);
	set_interrupt(IDT + 17, (dword) &_INT17, 0, KERNEL_CS);
	set_interrupt(IDT + 18, (dword) &_INT18, 0, KERNEL_CS);
	set_interrupt(IDT + 19, (dword) &_INT19, 0, KERNEL_CS);
	
	/* Interrupciones del timertick y del teclado respectivamente. */
	set_interrupt(IDT + 32, (dword) &_INT32, 0, KERNEL_CS);
	set_interrupt(IDT + 33, (dword) &_INT33, 0, KERNEL_CS);
	
	/* Llamadas al sistema. */
	set_interrupt(IDT + 0x80, (dword) &_INT80, 3, KERNEL_CS);
}







void handler_de_interrupciones(word nro_int){
	char* mensajes[20] = {	"Int 0h :                 #DE (Error de division)",
				"Int 1h : 			  #DB (Reservado)",
				"Int 2h : 			 Interrupcion NMI",
				"Int 3h : 			 #BP (Breakpoint)",
				"Int 4h : 		           #OF (Overflow)",
				"Int 5h : 	      #BR (Rango limite excedido)",
				"Int 6h :      #UD (Codigo de operacion invalido)",
				"Int 7h : 	  #NM (Dispositivo no disponible)",
				"Int 8h : 			#DF (Doble falta)",
				"Int 9h :     (Segmento de coprocesador excedido)",
				"Int Ah : 		       #TS (TSS Invalida)",
				"Int Bh : 	       #NP (Segmento no presente)",
				"Int Ch : 	  #SS (Fallo de segmento de pila)",
				"Int Dh : 		 #GP (Proteccion general)",
				"Int Eh : 		    #PF (Fallo de pagina)",
				"Int Fh : 		-- (Reservado para intel)",
				"Int 10h :	 #MF (Error matematico en la FPU)",
				"Int 11h : 	      #AC (Chequeo de alineacion)",
				"Int 12h : 	         #MC (Chequeo de maquina)",
				"Int 13h : #XM (Excepcion de punto flotante SIMD)"
			     };
	//PONGO LA PANTALLA EN AZUL	
	word* puntero = (word*) DIR_INI_PANTALLA;
	while(puntero < DIR_FIN_PANTALLA){
		*puntero = (AZUL_F | BRILLANTE) << 8;
		puntero++;
	}
	
	mover_puntero(12,16);
	printf(mensajes[nro_int], AZUL_F | BRILLANTE | GRIS_L);
	
	while(TRUE);
}

