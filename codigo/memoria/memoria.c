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
