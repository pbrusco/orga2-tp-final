#ifndef __GDT_H__
#define __GDT_H__


//GLOBAL DESCRIPTOR TABLE - DEFINICION DE LA GDT, DEL GDTR Y DE LOS SEGMENTOS DESCRIPTORES DE LA GDT.


//Maximo fijado para la cantidad de Segmentos Descriptores en la GDT
#define GDT_COUNT 128

//Valores de los campos ATRIBUTOS del Segmento Descriptor de GDT
#define PRESENTE	 	1 << 7
#define DPL_0    		0 << 6
#define DPL_1    		1 << 5
#define DPL_2    		2 << 5
#define DPL_3   		3 << 5
#define GENERAL  		1 << 4
#define SISTEMA  		0 << 4
#define GRANULARIDAD    1 << 7
#define DB              1 << 6

//Valores del campo GENERAL del Segmento Descriptor de GDT
#define DATOS           0x2
#define EXPAND_DOWN     1 << 2
#define WRITE           1 << 1
#define CODIGO          0xA
#define CONFORMING      1 << 2
#define READ_GDT        1 << 1
#define ACCESED         1 << 0

//Valors del campo SISTEMA del Segmento Descriptor de GDT 
#define LDT             2
#define TASK_GATE       5
#define TSS_AVAILABLE   9
#define TSS_BUSY        0xB
#define CALL_GATE       0xC
#define INT_GATE        0xE
#define TRAP_GATE       0xF


//Tipo Registro de GDT 
typedef struct gdt_desc {
	word limite;
	dword base;
} __attribute__((__packed__)) gdt_desc;

//Tipo Segmento Descriptor de GDT
typedef struct gdt_entry {
	word limite1;		// Limite 0:15
	word base1;			// Base 0:15
	byte base2;			// Base 16:23
	byte atr1;			// Presente (1 bit), DPL (2 bits),Sistema(1bit), Tipo (4bits)
	byte atr2;			// Granularidad (1 bit), D/B (1 bit), 64bit_code (0), Available (1bit), Limite 16:19 (4 bits) 
	byte base3;			// Base  24:31
} __attribute__((__packed__, aligned (8))) gdt_entry;


//Declaracion de la GDT y su registro descriptor
extern gdt_entry gdt[];		
extern gdt_desc GDT_DESC;			
gdt_entry make_descriptor(dword base, dword limite, byte atr1, byte atr2);


//funciones para modificar/manipular la GDT 

short int buscar_entradaGDT_vacia();
void borrar_gdt_entry(word i);



#endif //__GDT_H__
