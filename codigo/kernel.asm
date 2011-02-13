BITS 16		; le indico al compilador que el codigo que prosigue es de 16 bits

global start
extern GDT_DESC
extern contarMemoria
extern iniciar_paginacion_kernel
extern llenarBitmap
extern console

extern IDT_DESC
extern idtFill
extern iniciar_BCP
extern iniciar_tss_kernel

%define KORG 0x1200				; posicion de inicio de kernel
%define DIRINIT 0x100000			; posicion de inicio del directorio de paginas
%define INICIO_TAREAS 0x2000			; posicion de inicio de las tareas estaticas
%define FIN_TAREAS 0xD000			; posicion de fin de las tareas estaticas (por ahora)

start:

	; deshabilito interrupciones
	cli


	; habilito el Gate A20 y checkeo que este habilitado
	call enable_A20
	call check_A20


	; cargo en el registro LGDT la direccion base de la GDT que armamos en gts.asm
	lgdt[GDT_DESC]

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
	mov ebp, 0x1FFFFF
	mov esp, 0x1FFFFF


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



    ; Habilito las interrupciones
	sti

	jmp $


; incluimos en el kernel el codigo de los siguientes archivos
	%include "macros/a20.asm"




;  rellenamos con 0's hasta la posicion donde inicia el bloque de tareas estaticas (0x4000)
	TIMES INICIO_TAREAS - KORG - ($ - $$) db 0x00

	incbin "0x2000.tsk"
	incbin "0x3000.tsk"

; por ahora relleno con 0's hasta donde habría tareas (10 tareas por ahora) suponiendo que cada una ocupa 4kb
	TIMES FIN_TAREAS - KORG - ($ - $$) db 0x00

