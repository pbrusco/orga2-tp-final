BITS 32


extern console
extern switch_task
extern exit

extern handler_de_interrupciones


; ----------------------------------------------------------------
; Interrupt Service Routines
; ----------------------------------------------------------------




;ESTA MACRO SIRVE PARA DEFINIR UNA RUTINA DE SERVICIO DE INTERRUPCION

%macro ISR 1		;EL PARAMETRO ES EL NUMERO DE RUTINA QUE ESTOY DEFINIENDO
global _INT%1

_INT%1:
	cli
	pushad
	push WORD %1
	call handler_de_interrupciones
	add esp, 2
	xchg bx, bx
	popad
	sti
	iret

%endmacro


; definimos las rutinas de atencion de interrupciones/excepciones propias del procesador
; lo unico que hacen es mostrar en pantalla que error se produjo y colgar la ejecucion

;ACA DEFINO LAS ISR, UTILIZANDO LA MACRO ANTES HECHA Y AYUDANDOME DE LA DIRECTIVA REP PARA NO ESCRIBIR TANTAS VECES LO MISMO
%assign i 0
%rep	20
	ISR i
	%assign i i+1
%endrep


;DEFINO LAS QUE FALTAN (NO LAS DEFINO ANTES PORQUE REQUIEREN DE UN TRATO DISTINTO)

; rutina de atencion de interrupcion del timertick y cambio de tarea

	global _INT32
	_INT32:
		cli				;deshabilito las interrupciones
		pushad
		mov al, 0x20
		out 0x20, al			;aviso al pic que se atendio la interrupcion
		call switch_task
		popad
		sti
		iret


; rutina de atencion de interrupcion del teclado
	global _INT33
	_INT33:
		cli								;deshabilito interrupciones
		pushad

		xor eax,eax
		in al,0x60							;levanto el byte proveniente del teclado
		push ax
    		call console
		add esp,2
		mov al, 0x20
		out 0x20, al							;aviso al pic que se atendio la interrupcion

		popad
		sti								;habilito las interrupciones
		iret								;vuelvo de la interrupcion

; rutina de atencion de llamadas al sistema
;TODO: SOLO SE ESTA ATENDIENDO LA LLAMADA DE "FIN DE TAREA". CUANDO SE AGREGUEN MAS LLAMADAS, ESTO SE VA A TENER QUE MODIFICAR

	global _INT80
	_INT80:
		cli
		pushad
		call exit
		sti
		iret

