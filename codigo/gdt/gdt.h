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
#define AVAILABLE       1 << 4
#define L_32            0 << 1
#define L_64            1 << 1
//Valores del campo GENERAL del Segmento Descriptor de GDT
#define DATOS           0x2
#define EXPAND_DOWN     1 << 2
#define WRITE_GDT       1 << 1
#define CODIGO          0xA
#define CONFORMING      1 << 2
#define READ_GDT        1 << 1
#define ACCESED         1 << 0

//Valores del campo SISTEMA del Segmento Descriptor de GDT
#define LDT             2
#define TASK_GATE       5

#define TSS_BUSY        0xB
#define CALL_GATE       0xC
#define INT_GATE        0xE
#define TRAP_GATE       0xF

//Valores de los campos ATRIBUTOS Descriptor de GDT de TSS
#define TSS_AVAILABLE       9
#define TSS_0_OBLIGATORIO  0 << 4
#define TSS_GRANULARIDAD    1 << 7




//otras constantes
#define TAM_TSS         0x67

//Tipo Registro de GDT
typedef struct gdt_desc {
	uint16 limite;
	uint32 base;
} __attribute__((__packed__)) gdt_desc;

//Tipo Segmento Descriptor de GDT
typedef struct gdt_entry {
	uint16 limite1;		// Limite 0:15
	uint16 base1;			// Base 0:15
	uint8 base2;			// Base 16:23
	uint8 atr1;			// Presente (1 bit), DPL (2 bits),Sistema(1bit), Tipo (4bits)
	uint8 atr2;			// Granularidad (1 bit), D/B (1 bit), 64bit_code (0), Available (1bit), Limite 16:19 (4 bits)
	uint8 base3;			// Base  24:31
} __attribute__((__packed__, aligned (8))) gdt_entry;



//funciones para modificar/manipular la GDT
gdt_entry make_descriptor(uint32 base, uint32 limite, uint8 atr1, uint8 atr2);
uint16 buscar_entradaGDT_vacia();
void borrar_gdt_entry(uint16 i);
void info_GDT();


#endif //__GDT_H__

