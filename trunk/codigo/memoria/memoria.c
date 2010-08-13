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

}


//VERIFICAR QUE FUNCIONE!!!
void llenarBitmap(){

	//calculo cuantas paginas ocupa el directorio y las tablas de paginas del kernel
	unsigned long long offset_tablas = (memoria_total / 4) + 1;

	//calculo donde arranca el bitmap, justo debajo de las tablas de paginas
	dir_init_bitmap = (byte *) MB + offset_tablas;

	//empiezo a rellenar el bitmap
	int i = 0;
	int paginas_ocupadas = (DIR_KERNEL_END / TAM_PAG);
	int paginas_libres = ((memoria_total * MB) / TAM_PAG) - paginas_ocupadas;
	
	byte* bitmap = dir_init_bitmap;
	
	
	while(i < paginas_ocupadas/8)
	{
		*bitmap = 0xFF;
		bitmap++;
		i++;
	}	
	while(i < paginas_libres/8)
	{
		*bitmap = 0x00;
		bitmap++;
		i++;
	}	
}
