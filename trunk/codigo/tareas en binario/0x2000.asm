ORG 0x2000
BITS 32


main:
; Funcion para dibujar el reloj.

		xchg bx, bx
	next_clock:
		mov ah, 4
		mov ebx, [isrnumero]
		cmp ebx, 0x4
		jl .ok		
		mov DWORD [isrnumero], 0x0
		jmp next_clock	
		.ok:
			add ebx, isrmessage1
			mov al, [ebx]
			mov [0xb8400], ax
			inc DWORD [isrnumero]
			jmp next_clock

	isrnumero: dd 0x00000000
	isrmessage1: db '|'
	isrmessage2: db '/'
	isrmessage3: db '-'
	isrmessage4: db '\'

;TIMES 0x1000 - ($ - $$) db 0x00
