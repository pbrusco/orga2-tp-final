#include "../tipos/tipos_basicos.h"
#include "isr.h"
#include "idt.h"
#include "../pantalla/pantalla.h"

/*********************************************************************************\
\****** DECLARO LAS ESTRUCTURAS NECESARIAS PARA ATENDER LAS INTERRUPCIONES *******\
\*********************************************************************************/

IDT_Entry IDT[256] = {};

IDT_Descriptor IDT_DESC = {sizeof(IDT)-1, (uint32)&IDT};


void set_interrupt(IDT_Entry* idt, uint32 handler, uint16 dpl, uint16 segmento){
	idt->offset_0_15 = (uint16) (handler & 0x0000FFFF);
	idt->offset_16_31 = (uint16) (handler >> 16);
	idt->segsel = segmento;
	idt->ceros = 0x00;
	idt->tipo = 0x0E;
	idt->dpl = dpl;
	idt->presente = 1;	
}

void set_trap(IDT_Entry* idt, uint32 handler, uint16 dpl, uint16 segmento){
	idt->offset_0_15 = (uint16) (handler & 0x0000FFFF);
	idt->offset_16_31 = (uint16) (handler >> 16);
	idt->segsel = segmento;
	idt->ceros = 0x00;
	idt->tipo = 0x0F;
	idt->dpl = dpl;
	idt->presente = 1;	
}

void set_task_gate(IDT_Entry* idt, uint32 handler, uint16 dpl, uint16 segmento){
	idt->offset_0_15 = 0;
	idt->offset_16_31 = 0;
	idt->segsel = segmento;
	idt->ceros = 0x00;
	idt->tipo = 0x05;
	idt->dpl = dpl;
	idt->presente = 1;	
}



void idtFill() {
	
	/* Las primeras 20 interrupciones son las default. */
	set_interrupt(IDT + 0, (uint32) &_INT0, 0, KERNEL_CS);
	set_interrupt(IDT + 1, (uint32) &_INT1, 0, KERNEL_CS);
	set_interrupt(IDT + 2, (uint32) &_INT2, 0, KERNEL_CS);
	set_interrupt(IDT + 3, (uint32) &_INT3, 0, KERNEL_CS);
	set_interrupt(IDT + 4, (uint32) &_INT4, 0, KERNEL_CS);
	set_interrupt(IDT + 5, (uint32) &_INT5, 0, KERNEL_CS);
	set_interrupt(IDT + 6, (uint32) &_INT6, 0, KERNEL_CS);
	set_interrupt(IDT + 7, (uint32) &_INT7, 0, KERNEL_CS);
	set_interrupt(IDT + 8, (uint32) &_INT8, 0, KERNEL_CS);
	set_interrupt(IDT + 9, (uint32) &_INT9, 0, KERNEL_CS);
	set_interrupt(IDT + 10, (uint32) &_INT10, 0, KERNEL_CS);
	set_interrupt(IDT + 11, (uint32) &_INT11, 0, KERNEL_CS);
	set_interrupt(IDT + 12, (uint32) &_INT12, 0, KERNEL_CS);
	set_interrupt(IDT + 13, (uint32) &_INT13, 0, KERNEL_CS);
	set_interrupt(IDT + 14, (uint32) &_INT14, 0, KERNEL_CS);
	set_interrupt(IDT + 15, (uint32) &_INT15, 0, KERNEL_CS);
	set_interrupt(IDT + 16, (uint32) &_INT16, 0, KERNEL_CS);
	set_interrupt(IDT + 17, (uint32) &_INT17, 0, KERNEL_CS);
	set_interrupt(IDT + 18, (uint32) &_INT18, 0, KERNEL_CS);
	set_interrupt(IDT + 19, (uint32) &_INT19, 0, KERNEL_CS);
	
	/* Interrupciones del timertick y del teclado respectivamente. */
	set_interrupt(IDT + 32, (uint32) &_INT32, 0, KERNEL_CS);
	set_interrupt(IDT + 33, (uint32) &_INT33, 0, KERNEL_CS);
	
	/* Llamadas al sistema. */
	set_interrupt(IDT + 0x80, (uint32) &_INT80, 3, KERNEL_CS);
}







void handler_de_interrupciones(uint16 nro_int){
	int8* mensajes[20] = {	"Int 0h :                 #DE (Error de division)",
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
	uint16* puntero = (uint16*) DIR_INI_PANTALLA;
	while((uint32) puntero < DIR_FIN_PANTALLA){
		*puntero = (AZUL_F | BRILLANTE) << 8;
		puntero++;
	}
	
	mover_puntero(12,16);
	printf(mensajes[nro_int], AZUL_F | BRILLANTE | GRIS_L);
	
	while(TRUE);
}

