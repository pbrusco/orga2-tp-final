#include "gdt.h"
#include "../tareas/tss.h"

// Definicion de la GDT
gdt_entry gdt[GDT_COUNT] = {
    	
    	/* Descriptor de segmento nulo*/
	(gdt_entry){(long long int) 0},
    	
    	/* Descriptor de segmento de codigo*/
	(gdt_entry){ 
		(unsigned short) 0xFFFF,	//limit 0:15 	(16 bits)
		(unsigned short) 0x0000,	//base 0:15		(16 bits)
		(unsigned char) 0x00, 		//base 23:16	(7 bits)
		(unsigned char) 0xA, 		//type 			(4 bits)
		(unsigned char) 1, 			//system		(1 bit)
		(unsigned char) 0, 			//dpl			(2 bits)
		(unsigned char) 1, 			//present 		(1 bit)
		(unsigned char) 0xF,		//limit_16_19	(4 bit)
		(unsigned char) 0,  		//avaible		(1 bit)
		(unsigned char) 0,  		//l				(1 bit)
		(unsigned char) 1,  		//d/b			(1 bit)
		(unsigned char) 1, 			//granularity	(1 bit)
		(unsigned char) 0x00 		//base_31_24	(16 bits)
	},

    	/* Descriptor de segmento de datos*/
	(gdt_entry){ 
		(unsigned short) 0xFFFF,	//limit 0:15 	(16 bits)
		(unsigned short) 0x0000,	//base 0:15		(16 bits)
		(unsigned char) 0x00, 		//base 23:16	(7 bits)
		(unsigned char) 0x2, 		//type 			(4 bits)
		(unsigned char) 1, 			//system		(1 bit)
		(unsigned char) 0, 			//dpl			(2 bits)
		(unsigned char) 1, 			//presetn		(1 bit)
		(unsigned char) 0xF,		//limit_16_19	(4 bit)
		(unsigned char) 0,  		//avaible		(1 bit)
		(unsigned char) 0,  		//l				(1 bit)
		(unsigned char) 1,  		//d/b			(1 bit)
		(unsigned char) 1, 			//granularity	(1 bit)
		(unsigned char) 0x00 		//base_31_24	(16 bits)
	},
	
		/* Descriptor segmento de memoria de video*/
	(gdt_entry){ 
		(unsigned short) 0x0F9F,	//limit 0:15 	(16 bits)
		(unsigned short) 0x8000,	//base 0:15		(16 bits)
		(unsigned char) 0x0B, 		//base 23:16	(7 bits)
		(unsigned char) 0x2, 		//type 			(4 bits)
		(unsigned char) 1, 			//system		(1 bit)
		(unsigned char) 0, 			//dpl			(2 bits)
		(unsigned char) 1, 			//present		(1 bit)
		(unsigned char) 0x0,		//limit_16_19	(4 bit)
		(unsigned char) 0,  		//avaible		(1 bit)
		(unsigned char) 0,  		//l				(1 bit)
		(unsigned char) 1,  		//d/b			(1 bit)
		(unsigned char) 1, 			//granularity	(1 bit)
		(unsigned char) 0x00 		//base_31_24	(16 bits)
	},

		/* Descriptor de task 0*/
	(gdt_entry){ 
		(unsigned short) 0x0067, 	//limit 0:15 	(16 bits)
		(unsigned short) 0x0000,	//base 0:15		(16 bits)
		(unsigned char) 0x00, 		//base 23:16	(7 bits)
		(unsigned char) 0x9, 		//type 			(4 bits)
		(unsigned char) 0, 			//system		(1 bit)
		(unsigned char) 0, 			//dpl			(2 bits)
		(unsigned char) 1, 			//present		(1 bit)
		(unsigned char) 0x0,		//limit_16_19	(4 bit)
		(unsigned char) 0,  		//avaible		(1 bit)
		(unsigned char) 0,  		//l				(1 bit)
		(unsigned char) 0,  		//d/b			(1 bit)
		(unsigned char) 1, 			//granularity	(1 bit)
		(unsigned char) 0x00 		//base_31_24	(16 bits)
	}

};


void agregarTSSDescriptor(void* base){

short int lugarVacio = buscarLugarVacio();
gdt[lugarVacio] =	(gdt_entry){ 
		(unsigned short) 0x0067, 
		(unsigned short) (base & 0x0000FFFF),
		(unsigned char)  ((base & 0x00FF0000)>>16),
		(unsigned char) 0x9, 
		(unsigned char) 0, 
		(unsigned char) 0, 
		(unsigned char) 1, 
		(unsigned char) 0x0,
		(unsigned char) 0,  
		(unsigned char) 0,  
		(unsigned char) 0,  
		(unsigned char) 1, 
		(unsigned char)  (base>>24)
	}

}


short int buscarLugarVacio(){
int i = 1;
for (i;i<128;i++){
if (gdt[i] == 0) return i;
}
return 0;


}



void borrarTSSDescriptor(short int indice){

gdt[indice] =	(long long int) 0;	

}



// Definicion del GDTR
gdt_descriptor GDT_DESC = {sizeof(gdt)-1, (unsigned int)&gdt};



