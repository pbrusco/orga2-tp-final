ORG 0x0
BITS 32

main:
; Funcion para dibujar el reloj.
 ;xchg bx,bx
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
		mov al, 't'
		mov [0xB8500], ax
		mov al, 'e'
		mov [0xB8502], ax
		mov al, 'r'
		mov [0xB8504], ax
		mov al, 'm'
		mov [0xB8506], ax
		mov al, 'i'
		mov [0xB8508], ax
		mov al, 'n'
		mov [0xB850A], ax
		mov al, 'e'
		mov [0xB850C], ax
		mov al, '!'
		mov [0xB850E], ax
		mov al, '!'
		mov [0xB8510], ax
		int 0x80

	isrnumero: dd 0x00000000
	isrmessage1: db '|'
	isrmessage2: db '/'
	isrmessage3: db '-'
	isrmessage4: db '\'
	contador: dd 0

TIMES 0x200 - ($ - $$) db 0x00

