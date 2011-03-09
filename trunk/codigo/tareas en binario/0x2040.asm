ORG 0x2040
BITS 32


main:
; Funcion para dibujar el reloj.

	next_clock:
		inc DWORD [contador]
		cmp DWORD [contador], 0xFFFFFF
		je salir
		mov ah, 3
		mov ebx, [isrnumero]
		cmp ebx, 0x4
		jl .ok
		mov DWORD [isrnumero], 0x0
		jmp next_clock	
		.ok:
			add ebx, isrmessage1
			mov al, [ebx]
			mov [0xb8500], ax
			inc DWORD [isrnumero]
			jmp next_clock

	salir:
		int 0x80
		
	isrnumero: dd 0x00000000
	isrmessage1: db '|'
	isrmessage2: db '/'
	isrmessage3: db '-'
	isrmessage4: db '\'
	contador: dd 0

;TIMES 0x1000 - ($ - $$) db 0x00
