;#####################################################################	
;#                        EJERCICIO 1                                #
;#####################################################################


; imprimimos por pantalla mensajes de inicializacion	
	IMPRIMIR_MODO_REAL iniciando, iniciando_len, 0x07, 0xFFFF, 0
	IMPRIMIR_MODO_REAL modreal, modreal_len, 0x07, 0, 0
	IMPRIMIR_MODO_REAL cambiando, cambiando_len, 0x07, 0, 0

; habilito el Gate A20 y checkeo que este habilitado
	call enable_A20
	call check_A20
	
; cargo en el registro LGDT la direccion base de la GDT que armamos en gts.asm
	lgdt[GDT_DESC]			

; seteo el bit PE del registro de control CR0 para luego poder habilitar el modo protegido
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

; cargo en ES la posicion dentro de la GDT del descriptor del segmento de video
	mov ax, 0x18
	mov es, ax	


; a continuacion cambiamos el color del fondo de la pantalla al color negro
	mov ecx, (25*80) 		; ecx = tamaño de la matriz de video
	mov ah, 0x00			; ah = color de fondo(negro)
	mov al, 0x00			; al = caracter null 
	xor edi, edi			; edi = offset del segmento	

	.ciclo_negro:
		stosw			    ; guarda el contenido de eax en la posición apuntada por edi y avanza el puntero
		loop .ciclo_negro	; repetimos el ciclo hasta haber recorrido toda la matriz de video

	
; imprimimos por pantalla mensajes de sistema
	IMPRIMIR_TEXTO modprot, modprot_len, 0x07, 24, 59, 0xB8000
	
;#####################################################################	
;#                        EJERCICIO 2                                #
;#####################################################################


;cargo en el registro CR3 la direccion del Directorio de Tablas de Paginas del traductor y kernel
	mov eax, page_dir_traductorykernel	
	mov cr3, eax

; seteo el bit PG del registro de control CR0 para luego poder habilitar paginacion
	mov eax, cr0				
	or  eax, 0x80000000			
	mov cr0, eax

; escribo el nombre del grupo en la memoria de video a traves de la pagina 0x13000 del traductor y kernel, que esta 
; mapeada a la pagina de la memoria de video (0xB8000)	
	
	IMPRIMIR_TEXTO grupo, grupo_len, 0x07, 24, 0, 0x13000


;#####################################################################	
;#                        EJERCICIO 3                                #
;#####################################################################


; inicializo los descriptores de la IDT
	call idtFill

; remapeamos las interrupciones del vector de interrupciones 
	%include "macros/rutina_de_remapeo_pic.asm"
	
; cargo en el registro IDTR la direccion base y el limite de la IDT que armamos en idt.c
	lidt[IDT_DESC]



;#####################################################################	
;#                        EJERCICIO 4                                #
;#####################################################################


; Actualizo los descriptores de la GDT que apuntan a una tarea

	mov edi, gdt			; se posiciona en la primer entrada de tss de la gdt
	add edi, 0x20			

	; Tarea 0
	mov eax, tsss
	mov [edi+2], ax			; escribe la base_15:0 para el descriptor 0x20
	shr eax, 16
	mov [edi+4], al			; escribe la base_23:16 para el descriptor 0x20
	mov [edi+7], ah			; escribe la base_31:24 para el descriptor 0x20
	mov eax, tsss

	; Tarea pintor
	add eax, 104
	add edi, 8
	mov [edi+2], ax			; escribe la base_15:0 para el descriptor 0x28
	shr eax, 16
	mov [edi+4], al			; escribe la base_23:16 para el descriptor 0x28
	mov [edi+7], ah			; escribe la base_31:24 para el descriptor 0x28
	mov eax, tsss

	; Tarea traductor
	add eax, 208
	add edi, 8
	mov [edi+2], ax			; escribe la base_15:0 para el descriptor 0x30
	shr eax, 16
	mov [edi+4], al			; escribe la base_23:16 para el descriptor 0x30
	mov [edi+7], ah			; escribe la base_31:24 para el descriptor 0x30

; Cargo el registro de tareas (TR)
	mov ax, 0x20		
	ltr ax


; Habilito las interrupciones

	call tecladoFill
	
	sti
	
	
	jmp $
