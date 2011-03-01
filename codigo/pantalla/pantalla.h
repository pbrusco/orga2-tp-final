#ifndef __PANTALLA__
#define __PANTALLA__

#include "../tipos/tipos_basicos.h"


// el byte de atributos es asi
// 7(parpadeo), 654(fondo), 3(brillante), 210(letra)

// DEFINO LOS COLORES
#define NEGRO_L 0x0
#define AZUL_L 0x1
#define VERDE_L 0x2
#define CELESTE_L 0x3
#define ROJO_L 0x4
#define VIOLETA_L 0x5
#define NARANJA_L 0x6
#define GRIS_L 0x7

#define NEGRO_F 0x0 << 4
#define AZUL_F 0x1 << 4
#define VERDE_F 0x2 << 4
#define CELESTE_F 0x3 << 4
#define ROJO_F 0x4 << 4
#define VIOLETA_F 0x5 << 4
#define NARANJA_F 0x6 << 4
#define GRIS_F 0x7 << 4

// DEFINO LAS OPCIONES
#define PARPADEO 1 << 7
#define BRILLANTE 1 << 3

// BASES COMUNES
#define BASE10 (10 << 8)
#define BASE16 (16 << 8)

// DEFINO VALORES UTILES SOBRE LA PANTALLA
#define CANT_COLS 80
#define CANT_FILAS 25
#define DIR_INI_PANTALLA 0xB8000
#define DIR_FIN_PANTALLA 0xB8FA0

#define TAM_PANTALLA_TAREA 2*80*24

// FUNCIONES GLOBALES
void avanzar_puntero();
void mover_puntero(byte fila, byte columna);
void clear_screen();
void fill_random_screen();

/*Avanza el puntero de la pantalla una fila.*/
void salto_de_linea();

/*printf la vamos a usar sólo para imprimir caracteres. Imprime por defecto en GRIS con fondo NEGRO*/
void printf(const char *frase, const byte atrib);

/*printdword se usará para ver valores de variables.
Recibe como parámetros la dword a imprimir y los atributos de escritura (el byte alto es la base a usar y el bajo son los atributos de impresion, como el color, brillo, etc)*/
void printdword(const dword var, const word atr);


void printl(const char *frase, byte flag, byte atrib, dword param);
void putc(char c, byte atrib);
void num2char(dword n, byte* buffer, dword base);
void borrarc();
void retroceder_puntero();
int char2num(char c);
char num2char2(const int n);


// FUNCIONES PARA MOVIMIENTO DE PANTALLAS

/*TODO: PROBARLA   La idea es copiar los 24 renglones enteros de la tarea "pid". Recordar que la pantalla será de 80*24, de manera de dejarle el ultimo "renglon" para el kernel y la consola.
IMPORTANTE: ESTA FUNCION DEBE SER LLAMADA UNICAMENTE POR EL KERNEL, YA QUE ES EL UNICO QUE PUEDE VER Y MODIFICAR EL DIRECTORIO Y LAS TABLAS DE PAGINAS DE TODAS LAS TAREAS.
*/
void mostrar_pantalla_entera();

/*: Esta funcion modifica el valor de la variable "tarea_a_mostrar", que luego será utilizada por "mostrar_pantalla_entera" para hacer el cambio de pantalla correspondiente.
*/
void cambiar_de_pantalla(byte bcpPos);



#endif

