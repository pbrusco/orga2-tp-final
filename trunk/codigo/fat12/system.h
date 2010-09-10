/* system.h */

#ifndef __ROUTIX_TYPES
#include "sys/types.h"
#endif

#ifndef __SYSTEM
#define __SYSTEM

#define rdtscl(low) \
     __asm__ __volatile__ ("rdtsc" : "=A" (low))


/* Constantes de memoria física y virtual */
#define KERNEL_FCODE    0
#define KERNEL_FDATA    0x100000
#define KERNEL_FSTACK_TOP 0x200000      
#define KERNEL_STACK_SIZE 0x20000
#define KERNEL_FSTACK   KERNEL_FSTACK_TOP - KERNEL_STACK_SIZE

#define KERNEL_END      KERNEL_FSTACK_TOP

#define KERNEL_CODE             0xc0000000
#define KERNEL_DATA             0xc8000000
#define KERNEL_STACK            0xd0000000
#define KERNEL_STACK_TOP        KERNEL_STACK + 0x8000000
#define KERNEL_INTERRUPT        KERNEL_STACK_TOP


#define DESC_CODE               1<<3
#define DESC_DATA               2<<3
#define DESC_CODE_USUARIO       3<<3 | 3
#define DESC_DATA_USUARIO       4<<3 | 3
#define DESC_TSS0               5<<3
#define DESC_TSS1               6<<3

/* Variables Globales */
extern dword memoria;

extern addr_t KERNEL_PDT;
extern addr_t USER_PDT;

//Kernel PANIC (suceso inesperado o irrecuperable)
void kpanic(char *str);

#define nop() __asm__ __volatile__ ("nop")

#define cli() __asm__ __volatile__ ("pushf ; cli")
#define sti() __asm__ __volatile__ ("popf")

#define _cli()  __asm__ __volatile__ ("cli")
#define _sti()  __asm__ __volatile__ ("sti")

int getvar (char *);
int setvar (char *, int);
// Funciones de libreria
int sys_putc (char);
void kprintf ( char *string, ... );


#endif
// Datarate Select Register (DSR)
#define DSR_RESET               1<<7
#define DSR_POWERDOWN           1<<6


// Comandos
#define SENSE_INTERRUPT_STATUS  0x8

// CONFIGURE - son dos bytes que se deben enviar
#define CONFIGURE_0             0x13
#define CONFIGURE_1             0

// Opciones de CONFIGURE
#define CONF_EIS                1<<6
#define CONF_EFIFO              1<<5
#define CONF_POLL               1<<4

// SPECIFY
#define SPECIFY                 0x3

// RECALIBRATE
#define RECALIBRATE             0x7

// READ SECTOR ID
#define READ_SECTOR_ID          0x4a

// READ SECTOR
#define READ_SECTOR             0Xe6 // MT=1 MFM=1 SK=1

// WRITE SECTOR
#define WRITE_SECTOR            0x0

// SEEK
#define SEEK                    0xf


//Definiciones relacionadas a la fase de resultados
#define ST0_IC_OK           (0x0 << 6)
#define ST0_IC_ABNORMAL     (0x1 << 6)
#define ST0_IC_NOT_EXIST    (0x2 << 6)
#define ST0_IC_ABNORMAL_POLL (0x3 << 6)
#define ST0_SEEK            (0x1 << 5)
#define ST0_NOT_READY       (0x1 << 3)
#define ST0_HEAD
#define ST0_DRIVE_A         0x0
#define ST0_DRIVE_B         0x1
#define ST0_DRIVE_C         0x2
#define ST0_DRIVE_D         0x3

// Verifica el contenido de los bits IC del ST0
#define COMMAND_OK ( ! (ST0&0xc0 == ST0_IC_OK ) )


#define BLOQUE_SIZE 512

void set_dor(word valor);


// Definiciones de DMA 

#define DMA_MASK        0xA
#define DMA_CMD         0xB
#define DMA_CH2_PAGE    0x81
#define DMA_CH2_ADDR    0x4
#define DMA_CH2_COUNT   0x5
#define DMA_READ        0x46    // Floppy --> mem
#define DMA_WRITE       0x4A    // mem --> floppy

/* Inicializa el Canal 2 del DMA (usado para transferencias del floppy */
void init_floppy_DMA (byte);


// Definiciones del floppy

#define SECTORES_POR_PISTA      18
#define CARAS_POR_PISTA         2
#define MAX_FLOPPY_TRYS         1       // Cantidad maxima de intentos de lectura/escritura antes de tirar error

int leer_escribir(byte operacion, dword sector_logico, byte *buffer);

#define MOTOR_ON        1
#define MOTOR_OFF       0

#define TIEMPO_APAGADO_MOTOR_POR_INACTIVIDAD    2000000
#define TIEMPO_TIMEOUT_COMANDO                  1000000
#define TIME_WAIT_FOR_MOTOR_ON  500000




