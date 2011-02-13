BITS 32
%include "macros/macrosmodoprotegido.mac"
extern pic1_intr_end
extern console
extern switch_task
extern salto

; ----------------------------------------------------------------
; Interrupt Service Routines
; TODO: Definir el resto de las ISR
; ----------------------------------------------------------------

global _isrE, _isr20, _isr21

	msgisrE: db 'Int Eh : #PF (Fallo de pagina)'
	msgisrE_len equ $-msgisrE

	msgisr21: db 'Int 21h : Tecleaste Algo'
	msgisr21_len equ $-msgisr21


; definimos las rutinas de atencion de interrupciones/excepciones propias del procesador
; lo unico que hacen es mostrar en pantalla que error se produjo y colgar la ejecucion

	_isrE:
		cli
		mov edx, msgisrE
		IMPRIMIR_TEXTO edx, msgisrE_len, 0x0C, 0, 0, 0xb8000
		jmp $

; rutina de atencion de interrupcion del timertick y cambio de tarea

	_isr20:
		cli				;deshabilito las interrupciones
		mov al, 0x20
		out 0x20, al			;aviso al pic que se atendio la interrupcion
		call switch_task
		cmp al, 0
		je continuar
		jmp FAR [salto]
	continuar:
		sti
		iret


; rutina de atencion de interrupcion del teclado
; al igual que las del procesador, se muestra un mensaje por pantalla avisando que interrupcion se produjo, y continua la ejecucion

	_isr21:
		cli								;deshabilito interrupciones
		in al,0x60							;levanto el byte proveniente del teclado
		push ax

    call console
    add esp,2
    mov al, 0x20
		out 0x20, al							;aviso al pic que se atendio la interrupcion

		sti								;habilito las interrupciones
		iret								;vuelvo de la interrupcion

