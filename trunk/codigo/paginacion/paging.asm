;----------------------------------------------------------------------------------------------------------------------
; ACLARACION : 	Este archivo es incluido en el kernel en la posicion de memoria 0xA000. Con lo cual todo el codigo que sigue se presenta en memoria a partir de la direccion antes señalada.
;----------------------------------------------------------------------------------------------------------------------


; declaramos el Directorio de Tablas de Paginas del Pintor en la posicion 0xA000
page_dir_pintor:			

	;la primer entrada apunta a la tabla de pagina ubicada en 0xC000, supervisor|read/write|presente
		dd 	0x0000C003 		

	; las restantes 1023 entradas del Directorio apuntan a 0x0000 y estan no presentes
		%rep	0x400 - 1
			dd	0x00000002	;supervisor, read/write, not present
		%endrep


; declaramos el Directorio de Tablas de Paginas del Pintor en la posicion 0xB000
page_dir_traductorykernel:		

	;la primer entrada apunta a la tabla de pagina ubicada en 0xD000, supervisor|read/write|presente
		dd 	0x0000D003 		
	
	; las restantes 1023 entradas del Directorio apuntan a 0x0000 y estan no presentes
		%rep	0x400 - 1
			dd	0x00000002	;supervisor, read/write, not present
		%endrep



; declaramos la Tabla de Pagina de la tarea Pintor
page_table_pintor:

	; Pagina presente // 0000 - 8FFF --> 0000 - 8FFF
		%assign i 0x0000	
		%rep    0x9		
			dd i | 3		;supervisor, read/write, presente
		%assign i i+4096
		%endrep

	; Pagina NO presente // 9000 - DFFF --> 9000 - DFFF
		%assign i 0x9000
		%rep    0x5	
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	; Pagina presente // E000 - FFFF --> E000 - FFFF
		%assign i 0xE000
		%rep    0x2
			dd 	i | 3		;supervisor, read/write, present			
		%assign i i+4096 
		%endrep
		
	; Pagina NO presente // 10000 - 12FFF --> 10000 - 12FFF
		%assign i 0x10000			
		%rep    0x3				
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	; Pagina presente // 13000 - 13FFF --> B8000 - B8FFF	
		%assign i 0xB8000			
		%rep    0x1
			dd 	i | 3		;supervisor, read/write, present
		%assign i i+4096 
		%endrep
		
	; Pagina NO presente // 14000 - 14FFF --> 14000 - 14FFF
		%assign i 0x14000
		%rep    0x1
			dd 	i | 2		;supervisor, read/write, no presente
		%endrep
	
	; Pagina presente // 15000 - 15FFF --> 15000 - 15FFF
		%assign i 0x15000		
		%rep    0x1
			dd 	i | 3		;supervisor, read/write, present
		%endrep
		
	; Pagina NO presente // 16000 - B7FFF --> 16000 - B7FFF
		%assign i 0x16000	
		%rep    0xA2		
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	; Pagina presente // B8000 - B8FFF --> 10000 - 10FFF
		dd 	0x10003			;supervisor, read/write, present
	
	; Pagina NO presente // B9000 - 3FFFFF--> B9000 - 3FFFFF
		%assign i 0xB9000	
		%rep    0x347	
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep


; declaramos la Tabla de Pagina compartida entre la tarea Traductor y el kernel
page_table_traductorykernel:

	;0000 - 7FFF --> 0000 - 7FFF
		%assign i 0x0000
		%rep    0x8	
			dd i | 3		;supervisor, read/write, presente
		%assign i i+4096
		%endrep
		
	;8000 - 8FFF --> 8000 - 8FFF
		%assign i 0x8000	
		%rep    0x1	
		dd i | 2			;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	;9000 - 10FFF --> 9000 - 10FFF
		%assign i 0x9000	
		%rep    0x8	
			dd i | 3		;supervisor, read/write, presente
		%assign i i+4096
		%endrep
		
	;11000 - 12FFF --> 11000 - 12FFF
		%assign i 0x11000	
		%rep    0x2	
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	;13000 - 13FFF --> B8000 - B8FFF
		%assign i 0xB8000	
		%rep    0x1	
			dd i | 3		;supervisor, read/write, presente
		%assign i i+4096
		%endrep
		
	;14000 - 15FFF --> 14000 - 15FFF
		%assign i 0x14000	
		%rep    0x2	
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	;16000 - 16FFF --> 16000 - 16FFF
		%assign i 0x16000	
		%rep    0x1	
			dd i | 3		;supervisor, read/write, presente
		%assign i i+4096
		%endrep
		
	;17000 - 17FFF --> 17000 - 17FFF
		%assign i 0x17000	
		%rep    0x1	
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	;18000 - 18FFF --> B8000 - B8FFF
		%assign i 0xB8000
		%rep    0x1	
			dd i | 3		;supervisor, read/write, presente
		%assign i i+4096
		%endrep
		
	;19000 - 9FFFF --> 19000 - 9FFFF
		%assign i 0x19000	
		%rep    0x87	
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
		
	;A0000 - BFFFF --> A0000 - BFFFF
		%assign i 0xA0000	
		%rep    0x20	
			dd i | 3		;supervisor, read/write, presente
		%assign i i+4096
		%endrep
		
	;C0000 - 3FFFFF --> C0000 - 3FFFFF
		%assign i 0xC0000	
		%rep    0x340	
			dd i | 2		;supervisor, read/write, no presente
		%assign i i+4096
		%endrep
