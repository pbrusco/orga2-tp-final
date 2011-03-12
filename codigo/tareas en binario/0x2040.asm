ORG 0x2000
BITS 32

main:
; Funcion para dibujar el reloj.
	next_clock:
		inc DWORD [contador]
		cmp DWORD [contador], 0xFFFF
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
			mov [0xB8500], ax
			inc DWORD [isrnumero]
			jmp next_clock

	salir:
		mov DWORD [contador], 0
		int 0x80
		
	isrnumero: dd 0x00000000
	isrmessage1: db '|'
	isrmessage2: db '/'
	isrmessage3: db '-'
	isrmessage4: db '\'
	contador: dd 0

TIMES 0x200 - ($ - $$) db 0x00
