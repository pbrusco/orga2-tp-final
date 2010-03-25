BITS 16		; le indico al compilador que el codigo que prosigue es de 16 bits

global start
extern GDT_DESC
extern gdt
extern IDT_DESC
extern idtFill
extern tsss
extern tecladoFill

%define KORG 0x1200					; posicion de inicio de kernel
%define DIRINIT 0x8000				; posicion de inicio del directorio del kernel


; incluimos al principio del kernel el codigo de las macros para imprimir en pantalla en modo real
	%include "macros/macrosmodoreal.mac"
	%include "macros/macrosmodoprotegido.mac"

start:

; deshabilito interrupciones
	cli					
	jmp 	bienvenida

;  mensajes por pantalla del sistema
	iniciando: db 'Iniciando el kernel'
	iniciando_len equ $ - iniciando

	modreal: db 'Cpu en Modo Real'
	modreal_len equ $ - modreal

	cambiando: db 'Cambiando a Modo Protegido'	
	cambiando_len equ $ - cambiando

	modprot: db 'Cpu en Modo Protegido'
	modprot_len equ $ - modprot

	grupo db 'Grupo PUNPCKHQDQ',0
	grupo_len equ $-grupo


bienvenida:
	; ejercicio 1
	
		%include "ejercicios/ejercicios.asm"

		
		
; incluimos en el kernel el codigo de los siguientes archivos
	%include "macros/a20.asm"

;  rellenamos con 0's hasta la posicion donde inicia el directorio de paginas del kernel (0x8000)
	TIMES DIRINIT - KORG - ($ - $$) db 0x00




; incluimos el codigo de la tarea pintor , y como este ocupa justamente una pagina, seguidamente incluimos el codigo de la tarea traductor sin necesidad de tener que rellenar con 0's nuevamente
;	incbin "tareas/pintor.tsk"
;	incbin "tareas/traductor.tsk"

;  rellenamos con 0's hasta la posicion donde inicia el Directorio de Paginas de la tarea pintor (0xA000)
;	TIMES ComienzoDirectorioPaginaPintor - KORG - ($ - $$) db 0x00

; incluimos los Directorios de Tablas de Paginas y las Tablas de Paginas del kernel
	%include "paginacion/paging.asm"
	
	
