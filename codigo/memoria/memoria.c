#include "../tipos/tipos_basicos.h"
#include "memoria.h"

void contarMemoria(){
	//arranco suponiendo que el sistema cuenta con al menos 2 mb
	dword *dir = (dword *) (0x2*MB);
	dword dato = 0x12345678;

	*dir = dato;
	
	while(*dir == dato){
		*dir = 0;
		dir += TAM_PAG;
		*dir = dato;
	}
	
	memoria_total = ((dword) dir) / (1*MB);
	
	paginas_libres = ( (memoria_total * MB) - (2*MB) ) / TAM_PAG ;

}


//Arma el bitmap al inicio. Marca como ocupados los lugares reservados para el kernel
void llenarBitmap(){

	//calculo cuantas paginas ocupa el directorio y las tablas de paginas del kernel
	unsigned long long offset_tablas = (memoria_total / 4) + 1;

	//calculo donde arranca el bitmap, justo debajo de las tablas de paginas
	dir_init_bitmap = (byte *) MB + (offset_tablas * TAM_PAG);
	
	//calculo donde termina el bitmap (ultima dir valida del bitmap)
	dir_end_bitmap =  (byte *)   (((dword) dir_init_bitmap) + ((((memoria_total * MB) / TAM_PAG)/8) - 1));

	int pag_ocupadas = ((DIR_KERNEL_END) / TAM_PAG)/8;
	
	byte* bitmap = dir_init_bitmap;
	
	
	while(0 < pag_ocupadas)
	{
		*bitmap = 0xFF;
		bitmap++;
		pag_ocupadas--;
	}

	while(bitmap <= dir_end_bitmap)
	{
		*bitmap = 0x00;
		bitmap++;
	}
}

//Devuelve un puntero a una pagina vacia, y la marca como ocupada en el bitmap
dword* pidoPagina() {

	byte* bitmap_dir = dir_init_bitmap + (((2*MB)/TAM_PAG)/8);//comienzo del bitmap + offset ocupado
	byte libre;
	dword contador_paginas = 512;//desde los 2 MB en adelante
	dword var = 0;

	while(bitmap_dir <= dir_end_bitmap && *bitmap_dir == 0xFF){
		bitmap_dir++;
		contador_paginas = contador_paginas + 8;
	}

	if(bitmap_dir <= dir_end_bitmap) {

		libre = *bitmap_dir;
		 
		int i;
		for(i = 0;i<8;i++) {
			if ((libre & 1) == 0) {
				var = contador_paginas + i;
				*bitmap_dir = *bitmap_dir | (1 << i); //Se actualiza el BIT correspondiente a la pagina obtenida
				
				//actualizo la cant de paginas libres
				paginas_libres--;
				
				return (dword *) (var*4*1024);
			}
			libre = libre >> 1;
		}
	}
	
	return 0;
}

//Dada una dir de una pagina, la libera dentro del bitmap
void liberoPagina(dword* pagina){

	//me guardo el numero de la pagina que se quiere liberar, comenzando a contar desde los 2MB
	dword numero_de_pagina = (((int) pagina) / TAM_PAG) - (2*MB / TAM_PAG);
	
	//comienzo del bitmap + offset ocupado
	byte* bitmap_dir = dir_init_bitmap + (((2*MB)/TAM_PAG)/8);

	while(numero_de_pagina > 7){
		numero_de_pagina -= 8;
		bitmap_dir++;
	}
	
	byte mascara = 0x01;
	while(numero_de_pagina > 0){
		numero_de_pagina--;
		mascara = mascara << 1;
	}

	*bitmap_dir = *bitmap_dir & (mascara ^ 0xFF); // el operador ^ es el XOR, con FF invierto todos los bits
	
	//actualizo la cant de paginas libres
	paginas_libres++;
}

