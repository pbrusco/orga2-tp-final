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

#define TAM_PANTALLA_TAREA 2*80*23

//FUNCIONES DE DEBUGGING

/*Avanza el puntero de la pantalla un caracter.*/
void avanzar_puntero();
/*Retrocede el puntero de la pantalla un caracter.*/
void retroceder_puntero();
/*Mueve el puntero de la pantalla hasta fila y columna.*/
void mover_puntero(byte fila, byte columna);
/*Avanza el puntero de la pantalla una fila.*/
void salto_de_linea();
/*printf la vamos a usar sólo para imprimir caracteres.*/
void printf(const char *frase, const byte atrib);
/*printdword se usará para ver valores de variables.
Recibe como parámetros la dword a imprimir y los atributos de escritura (el byte alto es la base a usar y el bajo son los atributos de impresion, como el color, brillo, etc)*/
void printdword(const dword var, const word atr);
/*Escribe un caracter en la pantalla*/
void putc(char c, byte atrib);
/*Borra el ultimo caracter de la pantalla*/
void borrarc();
/*Deja en buffer un string que representa al numero pasado en "n" escrito en base "base".*/
void num2char(dword n, byte* buffer, dword base);
/*Limpia la pantalla.*/
void clear_screen();



// FUNCIONES PARA MOVIMIENTO DE PANTALLAS Y LA LINEA DE COMANDOS

#define FILA_INFORMACION 0
#define FILA_COMMAND_LINE 24

/*La idea es copiar 23 renglones enteros de la tarea "pid". Recordar que la pantalla será de 80*23, de manera de dejarle el ultimo "renglon" para el kernel y la consola y el primer renglón para saber qué estamos visualizando.
IMPORTANTE: ESTA FUNCION DEBE SER LLAMADA UNICAMENTE POR EL KERNEL, YA QUE ES EL UNICO QUE PUEDE VER Y MODIFICAR EL DIRECTORIO Y LAS TABLAS DE PAGINAS DE TODAS LAS TAREAS.
*/
void mostrar_pantalla_entera();

/*: Esta funcion modifica el valor de la variable "tarea_a_mostrar", que luego será utilizada por "mostrar_pantalla_entera" para hacer el cambio de pantalla correspondiente.
*/
void cambiar_de_pantalla(byte bcpPos);


/*Limpia la linea indicada*/
clear_line(byte linea);

/*Borra el ultimo caracter de la linea de comandos*/
removerc();

/*Escribe un caracter en la linea de comandos*/
agregarc();


/*
IDEA PARA LAS PANTALLAS: 
1) SOLO LAS 23 FILAS CENTRALES SON LAS QUE PODRAN USAR LAS TAREAS Y EL KERNEL PARA ESCRIBIR
2) EN LA PRIMER FILA SE INDICARA QUE SE ESTA VIENDO POR LA PANTALLA
3) EN LA ULTIMA FILA SE VERA LA CONSOLA CON LO QUE SE VAYA TIPEANDO
4) CUANDO SE PRESIONE ENTER, LA ULTIMA LINEA SE LIMPIA
*/









void printl(const char *frase, byte flag, byte atrib, dword param);
int char2num(char c);
char num2char2(const int n);




#endif

