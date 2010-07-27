ORG 0x9000
BITS 32

;mapear 0x18000 a 0xB8000 -> entrada 0x18 de la tabla
%define DEBUG
%define OFFSET_TICK (24 * 80 << 1) + (79 << 1)
main:
	mov esi, 0x10000
	mov edi, 0x18000
	mov ebx, 0xB8000
	mov edx, 21
	add edi, 320 + 2
	.ciclofilas:
		mov ecx, 78
		.ciclocolumnas:
			mov ax, [esi]
			;mov al, 'b'
			xor ax, bx
		
			;xchg ah, al
			mov [edi], ax
			inc esi
			inc esi

			inc edi
			inc edi
			inc ebx
			inc ebx
			loop .ciclocolumnas
			
			add edi, 4
			;add edx, 4
			dec edx
			cmp edx, 0
			jg .ciclofilas

%ifdef DEBUG
	hlt
	pushad
	mov edi, 0x13000
	inc DWORD [num]
	mov ebx, [num]
	cmp ebx, 0x4
	jl .ok
		mov DWORD [num], 0x0
		mov ebx, 0
	.ok:
	add ebx, msg1
	mov al, [ebx]
	mov ah, 0x0E
	mov WORD [edi + OFFSET_TICK], ax
	popad
%endif

	
	jmp main 

%ifdef DEBUG
	num: dd 0x00000000
	msg1: db '|'
	msg2: db '/'
	msg3: db '-'
	msg4: db '\'
%endif

	
;TIMES 0x1000 - ($ - $$) db 0x00
