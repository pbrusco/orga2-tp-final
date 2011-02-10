BITS 32
%include "macros/macrosmodoprotegido.mac"
extern pic1_intr_end



; ----------------------------------------------------------------
; Interrupt Service Routines
; TODO: Definir el resto de las ISR
; ----------------------------------------------------------------

global _isr0, _isr1, _isr2, _isr3, _isr4, _isr5, _isr6, _isr7, _isr8, _isr9, _isrA, _isrB, _isrC, _isrD, _isrE, _isrF, _isr10, _isr11, _isr12, _isr13, _isr20, _isr21

; definimos todos los mensajes de error que pueden ocurrir en el procesador
	msgisr0: db 'Int 0h : #DE (Error de division)'
	msgisr0_len equ $-msgisr0
	
	msgisr1: db 'Int 1h : #DB (Reservado)'
	msgisr1_len equ $-msgisr1
	
	msgisr2: db 'Int 2h : Interrupcion NMI'
	msgisr2_len equ $-msgisr2
	
	msgisr3: db 'Int 3h : #BP (Breakpoint)'
	msgisr3_len equ $-msgisr3
	
	msgisr4: db 'Int 4h : #OF (Overflow)'
	msgisr4_len equ $-msgisr4
	
	msgisr5: db 'Int 5h : #BR (Rango limite excedido)'
	msgisr5_len equ $-msgisr5
	
	msgisr6: db 'Int 6h : #UD (Codigo de operacion invalido)'
	msgisr6_len equ $-msgisr6
	
	msgisr7: db 'Int 7h : #NM (Dispositivo no disponible)'
	msgisr7_len equ $-msgisr7
	
	msgisr8: db 'Int 8h : #DF (Doble falta)'
	msgisr8_len equ $-msgisr8
	
	
	msgisr9: db 'Int 9h : # (Segmento de coprocesador excedido)'
	msgisr9_len equ $-msgisr9
	
	msgisrA: db 'Int Ah : #TS (TSS Invalida)'
	msgisrA_len equ $-msgisrA
	
	msgisrB: db 'Int Bh : #NP (Segmento no presente)'
	msgisrB_len equ $-msgisrB
	
	msgisrC: db 'Int Ch : #SS (Fallo de segmento de pila)'
	msgisrC_len equ $-msgisrC
	
	msgisrD: db 'Int Dh : #GP (Proteccion general)'
	msgisrD_len equ $-msgisrD
	
	msgisrE: db 'Int Eh : #PF (Fallo de pagina)'
	msgisrE_len equ $-msgisrE
	
	msgisrF: db 'Int Fh : -- (Reservado para intel)'
	msgisrF_len equ $-msgisrF
	
	msgisr10: db 'Int 10h : #MF (Error matematico en la FPU)'
	msgisr10_len equ $-msgisr10
	
	msgisr11: db 'Int 11h : #AC (Chequeo de alineacion)'
	msgisr11_len equ $-msgisr11
	
	msgisr12: db 'Int 12h : #MC (Chequeo de maquina)'
	msgisr12_len equ $-msgisr12
	
	msgisr13: db 'Int 13h : #XM (Excepcion de punto flotante SIMD)'
	msgisr13_len equ $-msgisr13
	
	msgisr21: db 'Int 21h : Tecleaste Algo'
	msgisr21_len equ $-msgisr21


; definimos las rutinas de atencion de interrupciones/excepciones propias del procesador
; lo unico que hacen es mostrar en pantalla que error se produjo y colgar la ejecucion
	_isr0:
		cli
		mov edx, msgisr0
		IMPRIMIR_TEXTO edx, msgisr0_len, 0x0C, 0, 0, 0xb8000
		jmp $
		
	_isr1:
		cli
		mov edx, msgisr1
		IMPRIMIR_TEXTO edx, msgisr1_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr2:
		cli
		mov edx, msgisr2
		IMPRIMIR_TEXTO edx, msgisr2_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr3:
		cli
		mov edx, msgisr3
		IMPRIMIR_TEXTO edx, msgisr3_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr4:
		cli
		mov edx, msgisr4
		IMPRIMIR_TEXTO edx, msgisr4_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr5:
		cli
		mov edx, msgisr5
		IMPRIMIR_TEXTO edx, msgisr5_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr6:
		cli
		mov edx, msgisr6
		IMPRIMIR_TEXTO edx, msgisr6_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr7:
		cli
		mov edx, msgisr7
		IMPRIMIR_TEXTO edx, msgisr7_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr8:
		cli
		mov edx, msgisr8
		IMPRIMIR_TEXTO edx, msgisr8_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr9:
		cli
		mov edx, msgisr9
		IMPRIMIR_TEXTO edx, msgisr9_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isrA:
		cli
		mov edx, msgisrA
		IMPRIMIR_TEXTO edx, msgisrA_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isrB:
		cli
		mov edx, msgisrB
		IMPRIMIR_TEXTO edx, msgisrB_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isrC:
		cli
		mov edx, msgisrC
		IMPRIMIR_TEXTO edx, msgisrC_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isrD:
		cli
		mov edx, msgisrD
		IMPRIMIR_TEXTO edx, msgisrD_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	
	_isrE:
		cli
		mov edx, msgisrE
		IMPRIMIR_TEXTO edx, msgisrE_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isrF:
		cli
		mov edx, msgisrF
		IMPRIMIR_TEXTO edx, msgisrF_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr10:
		cli
		mov edx, msgisr10
		IMPRIMIR_TEXTO edx, msgisr10_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr11:
		cli
		mov edx, msgisr11
		IMPRIMIR_TEXTO edx, msgisr11_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr12:
		cli
		mov edx, msgisr12
		IMPRIMIR_TEXTO edx, msgisr12_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
	_isr13:
		cli
		mov edx, msgisr13
		IMPRIMIR_TEXTO edx, msgisr13_len, 0x0C, 0, 0, 0xb8000
		jmp $
	
; rutina de atencion de interrupcion del timertick y cambio de tarea

	_isr20: 
;		cli				;deshabilito las interrupciones
;		mov al, 0x20
;		out 0x20, al			;aviso al pic que se atendio la interrupcion
;		sti
;		iret
	
	
;**********************************************************************************
;	LO DE ABAJO SON PRUEBAS QUE HICE PARA VER QUE TODO ANDE BIEN.
;**********************************************************************************
	
		cli				;deshabilito las interrupciones
		mov al, 0x20
		out 0x20, al			;aviso al pic que se atendio la interrupcion
		cmp dword [selector], 0x30
		je tarea2
	tarea1:
		mov dword [selector], 0x30
		jmp FAR [offset]
		sti
		iret	
	tarea2:
		mov dword [selector], 0x38
		jmp FAR [offset]
		sti
		iret				

		offset: dd 0
		selector: dw 0x30
		
;*****************************************************************************************************

; rutina de atencion de interrupcion del teclado
; al igual que las del procesador, se muestra un mensaje por pantalla avisando que interrupcion se produjo, y continua la ejecucion

	_isr21: 
		cli								;deshabilito interrupciones
		cmp byte [cont], 0						;ignoro la primer pulsacion del esc
		jg salir
		push edx							;guardo edx
		mov edx, msgisr21					
		IMPRIMIR_TEXTO edx, msgisr21_len, 0x0C, 0, 0, 0xb8000		;escribo por pantalla el mensaje de interrupcion
		pop edx	
		inc byte [cont]
		jmp sigo
	salir:	mov byte [cont], 0
	sigo:	in al,0x60							;levanto el byte proveniente del teclado
		mov al, 0x20
		out 0x20, al							;aviso al pic que se atendio la interrupcion
	
		sti										;habilito las interrupciones
		iret									;vuelvo de la interrupcion


	cont: db 1
	tarea: db 0

