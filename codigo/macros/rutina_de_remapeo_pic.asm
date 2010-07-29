; Inicializacion PIC1
	mov al, 0x11 			;ICW1: IRQs activas por flanco, Modo cascada, ICW4 Si.
	out 0x20, al
	
	mov al, 0x20			;ICW2: INT base para el PIC1 Tipo 8.
	out 0x21, al
	
	mov al, 0x04 			;ICW3: PIC1 Master, tiene un Slave conectado a IRQ2
	out 0x21 ,al
	
	mov al, 0x01 			;ICW4: Modo No Buffered, Fin de Interrupcion Normal
	out 0x21, al 			;Deshabilitamos las Interrupciones del PIC1
	
	mov al, 0xFF 			;OCW1: Set o Clearel IMR
	out 0x21, al

; Inicializacion PIC2
              
	mov al, 0x11 			;ICW1: IRQs activas por flanco, Modo cascada, ICW4 Si.
	out 0xA0, al
	mov al, 0x28 			;ICW2: INT base para el PIC2 Tipo 028h.
	out 0xA1, al
	mov al, 0x02 			;ICW3: PIC2 Slave, IRQ2 es la linea que envia al Master 
	out 0xA1, al
	mov al, 0x01 			;ICW4: Modo No Buffered, Fin de Interrupcion Normal
	out 0xA1, al

; Habilito el PIC1
	mov al, 0x00
	out 0x21, al


;DEJO DESHABILITADO EL PIC2
	mov al,0xFF
	out 0xA1, al


