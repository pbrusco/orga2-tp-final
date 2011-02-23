BITS 16		; le indico al compilador que el codigo que prosigue es de 16 bits

global start, gdt_vector


extern contarMemoria
extern iniciar_paginacion_kernel
extern llenarBitmap

extern cargarTarea
extern kernel_infinito

extern IDT_DESC
extern idtFill
extern iniciar_BCP
extern iniciar_tss_kernel

%define KORG 0x1200				; posicion de inicio de kernel
%define DIRINIT 0x100000			; posicion de inicio del directorio de paginas
%define INICIO_TAREAS 0x2000			; posicion de inicio de las tareas estaticas
%define FIN_TAREAS 0xD000			; posicion de fin de las tareas estaticas (por ahora)
%define DEFINITIVA_PILA 0x1FFFFF
%define GDT_COUNT 128

start:

	; deshabilito interrupciones
	cli

	; habilito el Gate A20 y checkeo que este habilitado
	call enable_A20
	call check_A20


	; cargo en el registro LGDT la direccion base de la GDT que armamos en gts.asm
	lgdt[gdt_desc]


	;#########################################################################################
	; seteo el bit PE del registro de control CR0 para luego poder habilitar el modo protegido
	;#########################################################################################
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	; habilito modo protegido cargando en CS la posicion dentro de la GDT del descriptor del segmento de codigo
	jmp 0x08:modo_protegido


	; le indico al compilador que el codigo que prosigue es de 32 bits
	bits 32

modo_protegido:

	; actualizo los selectores para que apunten al descriptor del segmento de datos en la GDT
	mov ax, 0x10
	mov ds, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov es, ax

	;pongo la pila en los 2MB
	mov ebp, DEFINITIVA_PILA
	mov esp, DEFINITIVA_PILA


	call contarMemoria
	call iniciar_paginacion_kernel


	;####################################################################################
	;cargo en el registro CR3 la direccion del Directorio de Tablas de Paginas del kernel
	;####################################################################################
	mov eax, DIRINIT
	mov cr3, eax

	; seteo el bit PG del registro de control CR0 para luego poder habilitar paginacion
	mov eax, cr0
	or  eax, 0x80000000
	mov cr0, eax


	call llenarBitmap


	;################################
	;inicio las distintas estructuras
	;################################
	call iniciar_BCP;
	call iniciar_tss_kernel; crea descriptor de tss en la gdt y la entrada corresp en la tss


	; Cargo el registro de tareas (TR)
	mov ax, 0x28
	ltr ax



    ; inicializo los descriptores de la IDT
	call idtFill



    ; remapeamos las interrupciones del vector de interrupciones
	%include "macros/rutina_de_remapeo_pic.asm"



    ; cargo en el registro IDTR la direccion base y el limite de la IDT que armamos en idt.c
	lidt[IDT_DESC]

	;push dword 0x2000
	;call cargarTarea
	;add esp, 4
	;push dword 0x2040
	;call cargarTarea
	;add esp, 4


    ; Habilito las interrupciones
	sti

	call kernel_infinito



; incluimos en el kernel el codigo de los siguientes archivos
	%include "macros/a20.asm"

gdt_vector:
	;entrada nula
	dd 0
	dd 0

	;gdt codigo kernel
	dw 0xFFFF	;limite 15:0
	dw 0x0000	;base 15:0
	db 0x00		;base 23:16
	db 0x9A		;presente | dpl 0 | 1b(codigo/datos) | codigo
	db 0xCF		;granularidad | D/B | limite 19:16
	db 0		;limite 31:24

	;gdt datos kernel
	dw 0xFFFF	;limite 15:0
	dw 0x0000	;base 15:0
	db 0x00		;base 23:16
	db 0x92		;presente | dpl 0 | 1b(codigo/datos) | datos
	db 0xCF		;granularidad | D/B | limite 19:16
	db 0		;limite 31:24

	;gdt codigo usuario
	dw 0xFFFF	;limite 15:0
	dw 0x0000	;base 15:0
	db 0x00		;base 23:16
	db 0xFA		;presente | dpl 3 | 1b(codigo/datos) | codigo
	db 0xCF		;granularidad | D/B | limite 19:16
	db 0		;limite 31:24

	;gdt datos usuario
	dw 0xFFFF	;limite 15:0
	dw 0x0000	;base 15:0
	db 0x00		;base 23:16
	db 0xF2		;presente | dpl 3 | 1b(codigo/datos) | codigo
	db 0xCF		;granularidad | D/B | limite 19:16
	db 0		;limite 31:24


; relleno con 0's hasta donde termine la gdt (son 128 entradas menos las 5 que ya inicialice
	TIMES ((GDT_COUNT-5)*8) db 0x00
gdt_desc:
	dw (gdt_desc - gdt_vector -1)
	dd gdt_vector

;  rellenamos con 0's hasta la posicion donde inicia el bloque de tareas estaticas (0x4000)
	TIMES INICIO_TAREAS - KORG - ($ - $$) db 0x00

	incbin "tareas en binario/0x2000.tsk"
	incbin "tareas en binario/0x2040.tsk"

; por ahora relleno con 0's hasta donde habría tareas (10 tareas por ahora) suponiendo que cada una ocupa 4kb
	TIMES FIN_TAREAS - KORG - ($ - $$) db 0x00

