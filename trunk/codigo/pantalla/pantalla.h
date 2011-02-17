#ifndef __PANTALLA__
#define __PANTALLA__

#include "../tipos/tipos_basicos.h"

// el byte de atributos es asi
// 7(parpadeo), 654(fondo), 3(brillante), 210(letra)

// DEFINO LOS COLORES
#define NEGRO 0x0
#define AZUL 0x1
#define VERDE 0x2
#define CELESTE 0x3
#define ROJO 0x4
#define VIOLETA 0x5
#define NARANJA 0x6
#define GRIS 0x7

// DEFINO LAS OPCIONES
#define PARPADEO 1 << 7
#define BRILLANTE 1 << 3

// DEFINO VALORES UTILES SOBRE LA PANTALLA
#define CANT_COLS 80
#define CANT_FILAS 25
#define DIR_INI_PANTALLA 0xB8000
#define DIR_FIN_PANTALLA 0xB8FA0

// FUNCIONES GLOBALES
void avanzar_puntero();
void mover_puntero(byte fila, byte columna);
void clear_screen();
void fill_random_screen();
void printf(const char *frase, byte flag, byte atrib, dword param);
void putc(char c, byte atrib);
void num2char(dword n, byte* buffer, dword base);


// FUNCIONES PARA MOVIMIENTO DE PANTALLAS

/*TODO: El pid es el indice en la gdt de la tarea que se quiere ver por pantalla. La idea es copiar los 24 renglones enteros de la tarea "pid" Recordar que la pantalla será de 80*24, de manera de dejarle el ultimo "renglon" para el kernel y la consola.*/
void mostrar_pantalla_entera(word pid);

/*TODO: 
*/



#endif

