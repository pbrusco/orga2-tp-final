 * Revision: 22 Marzo 2004
 *    + Se migra la funcion (Floppy) llamada desde la rutina de atencion a interrupcion (intFloppy) a este archivo, 
 *      con lo que se hace la variable "continuar" (de ahora en mas llamada "floppy_continuar") como local al
 *      archivo (static).
 *
 *    + Se ajustan los valores de inicializacion del floppy (comandos CONFIGURE y SPECIFY) de la siguiente forma:
 *              Se habilita la fifo (propia del 82077a) con un tama#o de 10 bytes.
 *              Step rate interval:  4mseg
 *              Head Unload time  : 16mseg
 *              Head Load time    : 16mseg
 *              
 *    + Proxima revision:
 *              funciones de lectura y escritura
 *              conversion de sectores logicos a fisicos (LBA)
 * ----------------------------------------------------------------
 *
 * Revision: 24 Marzo 2004
 *
 *    + Se crea la funcion dump_status_register() con fin de poder dumpear y debugear el estado de los registros
 *      de estado (ST0 a ST3)
 *
 *    + En la funcion read_sector se reemplaza la verificacion de todos los bits del ST0 por una macro "COMMAND_OK"
 *      (definida en include/drivers/floppy.h) que solo chequea el valor del IC de este registro de estado.
 *      Esto es ya que cuando estamos en la condicion de sector=18 el valor de "H" devuelto por ST0 cambia (de 0 a 1
 *      por ejemplo), pienso que se debera que nos indica la proxima posicion de la cabeza para leer el proximo
 *      sector (el sector numero 18 es el ultimo sector de un track en una cara).
 *      Con este cambio se observa que pueden ejecutarse sin problemas todos los archivos de disco.
 */



#include <drivers/floppy.h>
#include <fs/blockcache.h>
#include <routix/device.h>
#include <routix/atomic.h>
#include <routix/8259.h>
#include <routix/kstdio.h>
#include <routix/timer.h>

#define OK  0

int block(void);
int floppy_get_result(void);
int leer_sec(byte cara, byte cilindro, byte sector , byte *buffer);
void dump_states(void);
void dump_status_register(int numero, byte valor);

void leer_escribir_new();
void init_floppy_block_new(void);
void recalibrate_block_new(void);
void motor_on_new_finish(timer_t *timer);
void seek_block_new();
void read_sector_block_new();
void error();
void error_por_timeout();




// Puntero a funcion para manejar las interrupciones
static void (*func)(void) = NULL;

// Solicitud en proceso
static block_cache_t *actual_request = NULL;
#define ACTUAL_REQUEST (actual_request)

//Contiene el estado del motor (0=apagado , 1=encendido)
static byte motor_status=MOTOR_OFF;
static byte floppy_calibrado=0;
static byte seek_status=0;
static byte rw_status=0;

// Timer para encendido y apagado de motor
static timer_t *timer_motor = NULL;
// Timer de deteccion de TIMEOUTs
static timer_t *timer_timeout = NULL;

// Array con los valores de estados y pcn en la fase de resultado.
// Valores validos: 0-255   ;   ERR_TIMEOUT     ; NO_VALIDO
static int status[12];  

#define ST0 status[0]
#define ST1 status[1]
#define ST2 status[2]
#define ST3 status[3]


// En esta direccion (640K - 1024) se ubica el bloque para transferencias DMA. La ubicacion de este bloque no puede
// quedar entre dos paginas (el tamaño de estas paginas es de 64K) es decir tiene que estar totalmente contenido dentro
// del mismo bloque.
addr_t dma_address=0xA0000 - 1024;


void motor_on()
{
    outportb(DOR, DMA_INT | MOTA);
    motor_status=1;
    dword demora=0x3ffff;   
    while (demora--);           //Esperar por el regimen permanente del motor
}



// Version de motor on con timers
void motor_on_new()
{
  timer_motor = create_timer(USECONDS_TO_TICKS(TIME_WAIT_FOR_MOTOR_ON), NULL, motor_on_new_finish, NULL);

  outportb(DOR, DMA_INT | MOTA);
}

// Regimen permanente alcanzado
void motor_on_new_finish(timer_t *timer)
{
  //kprintf("Pasaron 500mseg... regimen permanente alcanzado\n");
  motor_status=MOTOR_ON;

  // El timer ya no existe
  timer_motor = NULL;


  // Si fue como resultado de un pedido de lectura o escritura
  if ( ACTUAL_REQUEST != NULL )
    leer_escribir_new();

}


void motor_off()
{
    outportb(DOR, DMA_INT);
    motor_status=0;
    floppy_calibrado=0;
}


void motor_off_new(timer_t *timer)
{
  outportb(DOR, DMA_INT);
  motor_status=MOTOR_OFF;

  // Todo a su posicion original
  floppy_calibrado=0;
  seek_status=0;
  rw_status=0;

  //kprintf("motor_off_new: Apagando el motor\n");

  // Eliminamos el timer
  //clean_timer(timer);

  // No existe
  timer_motor = NULL;
}



void set_dor(word valor)
{
 outportb(DOR, valor);
}



// Envia un byte al registro de datos del controlador                       //
int floppy_sendbyte(byte valor)
{
 unsigned char msr;
 int timeout;

 for (timeout=SENDBYTE_TIMEOUT; timeout; timeout--) {

  // Leemos primero el MSR (Main Status Register)
  msr = inportb(MSR);

  // Si RQM=1 y DIO=0 ( Host puede escribir info)
  if ( (msr & RQM) && ~(msr & DIO)  ) {
    outportb(DATA, valor);
    break;
  }

 }

 // Si timeout llego a 0 no se pudo completar la escritura
 return timeout;
}


// Lee un byte del registro de datos del controlador //
int floppy_get_result()
{
 unsigned char msr;
 byte posicion=0;
 int timeout;

    for (timeout=GETBYTE_TIMEOUT; timeout; timeout--) 
    {
        // Leemos primero el MSR (Main Status Register)
        msr = inportb(MSR) & (RQM | CMDBUSY | DIO);

        // Si RQM=1, DIO=1 y CMD_BUSY=1 ( Host puede leer info)
        if ( msr == (RQM | CMDBUSY | DIO) )
        {
            status[posicion++]=inportb(DATA);
            continue;
        }
        //Si el controller dejo de estar ocupado (CMD_BUSY=0), no hay mas resultados que levantar, salida Ok
        else if ( msr == RQM )
        {
            status[posicion]=NO_VALIDO;
            return OK;
        }
    }

    // Si timeout llego a 0 no se pudo completar la escritura
    return ERR_TIMEOUT; 

}


// Inicializacion del floppy
int init_floppy()
{
    int i;

     // Reset
    outportb(DOR, 0);

    outportb(DOR, 0x0C);

    // Programamos la velocidad de datos (500 Kbps para 1.44MB)
    outportb(CCR, DR_500KBPS);

    //Esperamos la interrupcion
    block();

    // Recolectamos el estado de las interrupciones de los 4 drives
    // que soporta el controlador
    for (i=0; i<4; i++) {

        if ( ! floppy_sendbyte(SENSE_INTERRUPT_STATUS) ) {
          return ERR_TIMEOUT;
        }


        if (floppy_get_result()==ERR_TIMEOUT)     {
          return ERR_TIMEOUT;
        }

    }


 // Ahora reconfiguramos el controlador
 if ( ! floppy_sendbyte(CONFIGURE_0) ) {
        return ERR_TIMEOUT;
 }
 if ( ! floppy_sendbyte(CONFIGURE_1) ) {
        return ERR_TIMEOUT;
 }

 // COMENTARIO: me quedan dudas respecto a estos valores ya que
 //     CONF_EIS: habilitamos los seeks automaticos (implicitos)
 //     CONF_EFIFO: deshabilitamos los fifos !! (modo 8272a)
 //     CONF_POLL:  deshabilitamos el polling, esto esta oks
 //if ( ! floppy_sendbyte(CONF_EIS|CONF_EFIFO|CONF_POLL) ) {}
 if ( ! floppy_sendbyte( (CONF_EIS|CONF_POLL) | 0xa) ) {                // 10 bytes de threshold
        return ERR_TIMEOUT;
 }
 if ( ! floppy_sendbyte(0) ) {
        return ERR_TIMEOUT;
 }

 // Seguimos con SPECIFY
 if ( ! floppy_sendbyte(SPECIFY) ) {
        return ERR_TIMEOUT;
 } 

 if ( ! floppy_sendbyte(0xC1) ) {                       // Seteamos SRT=4mseg (0xC0) y HUT=16mseg (0x01)
        return ERR_TIMEOUT;
 }

 if ( ! floppy_sendbyte(0x8<<1) ) {             // Seteamos HLT=16mseg (0x08 << 1) y ND (Ahora esta en NON-DMA !!)
        return ERR_TIMEOUT;
 }

   //kprintf("Inicializacion Oks !!\n");

        return OK;   
}




// Inicializacion del floppy
void init_floppy_new()
{

  // Reset
  outportb(DOR, 0);

  outportb(DOR, 0x0C);

  // Programamos la velocidad de datos (500 Kbps para 1.44MB)
  outportb(CCR, DR_500KBPS);

  //Esperamos la interrupcion, le indicamos la nueva rutina de atencion de interrupcion
  //block();
  func=init_floppy_block_new;
}


void init_floppy_block_new(void)
{       
  int i;

  // Reseteamos a func a su valor default
  func=NULL;

  // Recolectamos el estado de las interrupciones de los 4 drives
  // que soporta el controlador
  for (i=0; i<4; i++) {

    if ( ! floppy_sendbyte(SENSE_INTERRUPT_STATUS) ) {
      //return ERR_TIMEOUT;
    }


    if (floppy_get_result()==ERR_TIMEOUT)     {
      //return ERR_TIMEOUT;
    }

  }


 // Ahora reconfiguramos el controlador
 if ( ! floppy_sendbyte(CONFIGURE_0) ) {
        //return ERR_TIMEOUT;
 }
 if ( ! floppy_sendbyte(CONFIGURE_1) ) {
        //return ERR_TIMEOUT;
 }

 // COMENTARIO: me quedan dudas respecto a estos valores ya que
 //     CONF_EIS: habilitamos los seeks automaticos (implicitos)
 //     CONF_EFIFO: deshabilitamos los fifos !! (modo 8272a)
 //     CONF_POLL:  deshabilitamos el polling, esto esta oks
 //if ( ! floppy_sendbyte(CONF_EIS|CONF_EFIFO|CONF_POLL) ) {}
 if ( ! floppy_sendbyte( (CONF_EIS|CONF_POLL) | 0xa) ) {                // 10 bytes de threshold
        //return ERR_TIMEOUT;
 }
 if ( ! floppy_sendbyte(0) ) {
        //return ERR_TIMEOUT;
 }

 // Seguimos con SPECIFY
 if ( ! floppy_sendbyte(SPECIFY) ) {
        //return ERR_TIMEOUT;
 } 

 if ( ! floppy_sendbyte(0xC1) ) {                       // Seteamos SRT=4mseg (0xC0) y HUT=16mseg (0x01)
        //return ERR_TIMEOUT;
 }

 if ( ! floppy_sendbyte(0x8<<1) ) {             // Seteamos HLT=16mseg (0x08 << 1) y ND (Ahora esta en NON-DMA !!)
        //return ERR_TIMEOUT;
 }

// Si fue como resultado de un pedido de lectura o escritura
if ( ACTUAL_REQUEST != NULL )
  leer_escribir_new();

}


// Recalibra la posicion de la cabeza a la pista 0
#define PCN     status[1]

int recalibrate()
{

    // Reseteamos el flag
    floppy_calibrado=0;

    // Habilita el motor A y el Drive 0
    motor_on();

    // Ejecutamos un RECALIBRATE 
    if ( ! floppy_sendbyte(RECALIBRATE) ) {     
                return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte(DRIVE_0) ) {                 // DS0=0 DS1=0 (Drive 0)
                return ERR_TIMEOUT;
    }

    if ( block()!=OK )  {
                return ERR_NEED_RESET;
    }

    // Sensamos el estado de interrupcion
    if ( ! floppy_sendbyte(SENSE_INTERRUPT_STATUS) ) {
                return ERR_TIMEOUT;
    }

    // Obtener resultados de ejecucion 
    if (floppy_get_result()==ERR_TIMEOUT)     {
                return ERR_TIMEOUT;
    }

    // Analizamos el resultado del comando 
    if ( ! COMMAND_OK ) {
                //kprintf("RECALIBRATE\n");
                dump_states();
                return ERR_NEED_RESET;
    }

    // Pudimos calibrarlo sin problemas
    floppy_calibrado=1;

    return OK;
}



        int recalibrate_new()
        {

          // Reseteamos el flag
          floppy_calibrado=0;

          // Ejecutamos un RECALIBRATE 
          if ( ! floppy_sendbyte(RECALIBRATE) ) {       
            return ERR_TIMEOUT;
          }

          if ( ! floppy_sendbyte(DRIVE_0) ) {                   // DS0=0 DS1=0 (Drive 0)
            return ERR_TIMEOUT;
          }

          //Esperamos la interrupcion, le indicamos la nueva rutina de atencion de interrupcion
          func=recalibrate_block_new;

          // Aca debemos agregar el timer de timeout
          timer_timeout = create_timer(USECONDS_TO_TICKS(TIEMPO_TIMEOUT_COMANDO), NULL, error_por_timeout, NULL);

                if ( getvar("debugfloppy")==1 )
                        kprintf("Envio de recalibrate_new !!\n");

          return OK;

        }

        void recalibrate_block_new(void)
        {
          // Reseteamos a func a su valor default
          func=NULL;

          // Eliminamos el timer de timeout
          clean_timer(timer_timeout);

          // El timer ya no existe
          timer_timeout = NULL;

                if ( getvar("debugfloppy")==1 )
                        kprintf("Recalibrado ok !!\n");

          // Sensamos el estado de interrupcion
          if ( ! floppy_sendbyte(SENSE_INTERRUPT_STATUS) ) {
            //return ERR_TIMEOUT;
            error();
            return;
          }

          // Obtener resultados de ejecucion 
          if (floppy_get_result()==ERR_TIMEOUT)     {
            //return ERR_TIMEOUT;
            error();
            return;
          }

          // Analizamos el resultado del comando 
          if ( ! COMMAND_OK ) {
            dump_states();
            //return ERR_NEED_RESET;
            error();
            return;
          }

          // Pudimos calibrarlo sin problemas
          floppy_calibrado=1;

          // Si fue como resultado de un pedido de lectura o escritura
          if ( ACTUAL_REQUEST != NULL )
            leer_escribir_new();

  //return OK;
}


byte read_sector_id (void)
{
    // Habilita el motor A y el Drive 0
    motor_on();

    // Ejecutamos un Read sector id
    if ( ! floppy_sendbyte(READ_SECTOR_ID) ) {  
        return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte(0) ) {                       // HD=0 (head number) , DS0=0 DS1=0 (Drive 0)
        return ERR_TIMEOUT;
    }

    block();

    // Sensamos el estado de interrupcion
/*    if ( ! floppy_sendbyte(SENSE_INTERRUPT_STATUS) ) {
        puts("Error, timeout enviando SENSE_INTERRUPT_STATUS\n");
        return;
    }
*/
    // Obtener resultados de ejecucion 

    if (floppy_get_result()==ERR_TIMEOUT)
    {
        return ERR_TIMEOUT;
    }

    byte posicion=0;
    while ( status[posicion] != NO_VALIDO && posicion < 12)
    {
        //kprintf("Resultado Read Id%d: %x\n", (posicion), status[posicion]);
        posicion++;
    }

    return 1;

}       


// Posicionar el cabezal sobre la pista indicada

byte seek (byte cara, byte pista)
{

    // Ejecutamos un Read sector
    if ( ! floppy_sendbyte(SEEK) ) {    
        return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte( (cara << 2) |DRIVE_0) ) {                   // HD=0 (head number) , DS0=0 DS1=0 (Drive 0)
        return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte(pista) ) {                   // Cilindro
            return ERR_TIMEOUT;
    }

    block();

    // Sensamos el estado de interrupcion
    if ( ! floppy_sendbyte(SENSE_INTERRUPT_STATUS) ) {
        return ERR_TIMEOUT;
    }

    // Obtener resultados de ejecucion 
    if (floppy_get_result()==ERR_TIMEOUT)     {
        return ERR_TIMEOUT;
    }

    // Analizamos el resultado del comando 
    if ( ! COMMAND_OK ) {
        //kprintf("SEEK !!!\n");
        dump_states();
        return ERR_TIMEOUT;
    }

    return OK;
}       


int seek_new (byte cara, byte pista)
{

  // Ejecutamos un Read sector
  if ( ! floppy_sendbyte(SEEK) ) {      
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte( (cara << 2) |DRIVE_0) ) {                     // HD=0 (head number) , DS0=0 DS1=0 (Drive 0)
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(pista) ) {                     // Cilindro
    return ERR_TIMEOUT;
  }

  //Esperamos la interrupcion, le indicamos la nueva rutina de atencion de interrupcion
  func=seek_block_new;

  // Aca debemos agregar el timer de timeout
  timer_timeout = create_timer(USECONDS_TO_TICKS(TIEMPO_TIMEOUT_COMANDO), NULL, error_por_timeout, NULL);

        if ( getvar("debugfloppy")==1 )
                kprintf("Envio de seek_new !!\n");

  return OK;
}

void seek_block_new()
{
  // Reseteamos a func a su valor default
  func=NULL;

        if ( getvar("debugfloppy")==1 )
                kprintf("Retorno de seek_block !!\n");

  // Eliminamos el timer de timeout
  clean_timer(timer_timeout);

  // El timer ya no existe
  timer_timeout = NULL;  

  //kprintf("Int de seek Ok !!\n");

  // Sensamos el estado de interrupcion
  if ( ! floppy_sendbyte(SENSE_INTERRUPT_STATUS) ) {
    //return ERR_TIMEOUT;
    error();
    return;
  }

  // Obtener resultados de ejecucion 
  if (floppy_get_result()==ERR_TIMEOUT)     {
    //return ERR_TIMEOUT;
    error();
    return;
  }

  // Analizamos el resultado del comando 
  if ( ! COMMAND_OK ) {
    dump_states();
    //return ERR_TIMEOUT;
    error();
    return;
  }

  //kprintf("seek_block_new: seek ok!!\n");

  // Pudimos calibrarlo sin problemas
  seek_status=1;

  // Si fue como resultado de un pedido de lectura o escritura
  if ( ACTUAL_REQUEST != NULL ) {
    //kprintf("seek_block_new: Volvemos a leer_escribir_new\n");
    leer_escribir_new();
  }

}       

// Lee un sector fisico (previamente el cabezal debe estar calibrado y posicionado)

int read_sector (byte cara, byte cilindro, byte sector)
{

    init_floppy_DMA (DMA_READ);

    // Ejecutamos un Read sector
    if ( ! floppy_sendbyte(READ_SECTOR) ) {     
        return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte(DRIVE_0 | (cara << 2)) ) {                   // HD=0 (head number) , DS0=0 DS1=0 (Drive 0)
        return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte(cilindro) ) {                        // Cilindro
        return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte(cara) ) {                    // Cabeza
        return ERR_TIMEOUT;
    }

    if ( ! floppy_sendbyte(sector) ) {                  // Sector
        return ERR_TIMEOUT;
    }
    if ( ! floppy_sendbyte(2) ) {                       // Tamaño sector: 128 * 2^x (si x=2-->Sector=512 bytes)
        return ERR_TIMEOUT;
    }
    if ( ! floppy_sendbyte( SECTORES_POR_PISTA ) ) {                    // Sectores por pista
        return ERR_TIMEOUT;
    }
    if ( ! floppy_sendbyte(27) ) {                      // GAP: 27 para 3 1/2  ;    32 para 5 1/4
        return ERR_TIMEOUT;
    }
    if ( ! floppy_sendbyte(0xff) ) {                    // Data Lengh
        return ERR_TIMEOUT;
    }

    if ( block()!=OK )  {
        //kprintf("Error read sector block\n");
        return ERR_NEED_RESET;
    }

    // Obtener resultados de ejecucion 
    if (floppy_get_result()==ERR_TIMEOUT)     {
        return ERR_TIMEOUT;
    }

    // Analizamos el resultado del comando 
    if ( ! COMMAND_OK ) {
        dump_states();
        return ERR_NEED_RESET;
    }

    return OK;
}       



int read_sector_new (byte cara, byte cilindro, byte sector)
{

  //kprintf("read_sector_new: inicializando Dma\n");

  init_floppy_DMA (DMA_READ);

  // Ejecutamos un Read sector
  if ( ! floppy_sendbyte(READ_SECTOR) ) {       
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(DRIVE_0 | (cara << 2)) ) {                     // HD=0 (head number) , DS0=0 DS1=0 (Drive 0)
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(cilindro) ) {                  // Cilindro
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(cara) ) {                      // Cabeza
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(sector) ) {                    // Sector
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(2) ) {                 // Tamaño sector: 128 * 2^x (si x=2-->Sector=512 bytes)
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte( SECTORES_POR_PISTA ) ) {                      // Sectores por pista
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(27) ) {                        // GAP: 27 para 3 1/2  ;    32 para 5 1/4
    return ERR_TIMEOUT;
  }

  if ( ! floppy_sendbyte(0xff) ) {                      // Data Lengh
    return ERR_TIMEOUT;
  }

  //Esperamos la interrupcion, le indicamos la nueva rutina de atencion de interrupcion
  func=read_sector_block_new;

  // Aca debemos agregar el timer de timeout
  timer_timeout = create_timer(USECONDS_TO_TICKS(TIEMPO_TIMEOUT_COMANDO), NULL, error_por_timeout, NULL);

  return OK;
}

void read_sector_block_new()
{

  // Reseteamos a func a su valor default
  func=NULL;

  // Eliminamos el timer de timeout
  clean_timer(timer_timeout);

  // El timer ya no existe
  timer_timeout = NULL;  

  // Obtener resultados de ejecucion 
  if (floppy_get_result()==ERR_TIMEOUT)     {
    //return ERR_TIMEOUT;
    error();
    return;
  }

  // Analizamos el resultado del comando 
  if ( ! COMMAND_OK )   {
    dump_states();
    //return ERR_NEED_RESET;
    error();
    return;
  }

  // Pudimos calibrarlo sin problemas
  rw_status=1;

  // Si fue como resultado de un pedido de lectura o escritura
  if ( ACTUAL_REQUEST != NULL )
    leer_escribir_new();

}       


char read_msr()
{
 unsigned char msr;

 msr = inportb(MSR);

 return msr;
}

// block: bloquea la ejecucion del proceso que la llama hasta recibir una interrupcion de floppy

static byte floppy_continuar;       // variable candado de block

int block(void)
{
    // Deshabilitamos las interrupciones        
    disable_irq(6);

    floppy_continuar=0;

    // TEMPORAL
    dword timeout=0xffffffff;   //Este timeout es momentaneo (sera reemplazado por un timer). Puede que en maquinas con
                                //micro de mas de 600Mhz haya que aumentarlo... 
    // Las habilitamos nuevamente
    enable_irq(6); 

    // Esperamos que aparezca la interrupcion
    while (!floppy_continuar) {
        timeout--;
        if (!timeout)   //En caso de no haber llegado la IRQ6, salir con ERR_TIMEOUT
            return ERR_TIMEOUT;
    }

    return OK;
}



// Inicializa el controlador DMA, tanto para leer como para escribir (segun comando)

void init_floppy_DMA ( byte comando )
{
    outportb( DMA_MASK, 0x06 );
    // Resetear el FlipFlop del DMA para que reconozca al primer byte enviado como un LSB
    outportb ( 0xC, 0);

    // Enviar comando (DMA_READ, DMA_WRITE)
    outportb( DMA_CMD, comando );

    // Seteo de la direccion. No debera estar por encima de los 16MB
    outportb ( DMA_CH2_ADDR, (byte) dma_address);
    outportb ( DMA_CH2_ADDR, (byte) (dma_address >> 8) );
    outportb ( DMA_CH2_PAGE, (byte) (dma_address >> 16) );

    outportb ( DMA_CH2_COUNT, (byte) ( (BLOQUE_SIZE-1) & 0xff) );
    outportb ( DMA_CH2_COUNT, (byte) ( (BLOQUE_SIZE-1) >> 8) );
    outportb ( DMA_MASK, 2);
}       




// Acepta como operacion READ_SECTOR y WRITE_SECTOR (quiza pueda aceptar format).
// Es la encargada de convertir un sector logico (o LBA) en el sector fisico (CHS) correspondiente.
// Inicializa el controlador de floppy y enciende el motor. En caso de time outs, intenta hasta MAX_TRYS

int leer_escribir(byte operacion, dword sector_logico, byte *buffer)
{
    byte intentos=0;
    byte cara, cilindro, sector;


intentar_nuevamente:    
    intentos++;
    if (intentos > MAX_FLOPPY_TRYS ) {
        //kprintf("No se puede leer el sector %d(superados %d intentos)\n", sector_logico, MAX_FLOPPY_TRYS);
        return ERR_MAX_TRYS;
    }


    if (motor_status==0) {
        init_floppy();
        motor_on();
    }


    if (floppy_calibrado==0)
        if ( recalibrate()!=OK )
            goto intentar_nuevamente;


    //Calcular el sector fisico (CHS) a traves del sector logico (LBA)
    cara = (sector_logico / SECTORES_POR_PISTA) % CARAS_POR_PISTA;
    cilindro = (sector_logico / SECTORES_POR_PISTA) /CARAS_POR_PISTA;
    sector = (sector_logico % SECTORES_POR_PISTA) + 1;

    // Esto no deberia ser necesario ya que configuramos el controlador con EIS (Implied Seek) 
    if ( seek(cara, cilindro) != OK) {
            goto intentar_nuevamente;
    }

    if (operacion == READ_SECTOR) {
        if ( leer_sec(cara, cilindro, sector, buffer)!=OK )
            goto intentar_nuevamente;
        return OK;
    }

    //No implementado
    if (operacion == WRITE_SECTOR)
                return OK;
        else return -1;
}       


// Nueva funcion
//
// El proceso de lectura implica los siguientes pasos:
//
// 1. Encender el motor (si fuese necesario)
//
//      a. Setear el bit del motor correspondiente en el registro DOR
//      b. Esperar 500 mseg aproximadamente de modo que se estabilice
//         la velocidad de giro del motor
//
// 2. Recalibracion (si fuese necesario)
//
//      a. Enviar comando
//      b. Esperar interrupcion
//
// 3. Seek en la cabeza, cilindro y sector correspondiente
//
//      a. Enviar comando
//      b. Esperar interrupcion
//
// 4. Lectura del sector
//
//      a. Enviar comando
//      b. Esperar interrupcion
//
// Bien, el problema ppal es que si uso el modelo tipo:
//
//  * envio comando
//  * cambio el puntero de interrupcion de floppy
//
// se desestructura el codigo y pierdo las variables locales.
//
// Por ejemplo, en la funcion de lectura al llamar a la funcion de SEEK, 
// se pierde la memoria de la funcion previa (lectura).
//
//
// La primer solucion propuesta es la siguiente:
//
// Se guardará como un puntero la funcion base de ejecucion, en el ejemplo anterior sera
// la de lectura, en el proceso de lectura se realizara:
//
// 1. El motor esta encendido ?
//      NO: ejecutar motor_on()
//
// Si no estaba encendido, motor_on se ejecutara y devolver el control al kernel mientras espera
// que se venza el timer de 500 mseg, la funcion de retorno de motor_on() establecera la variable
// global motor_status como encendida y llamara nuevamente a la funcion de lectura, quien ahora
// realizara los siguientes pasos:
//
// 1. El motor esta encendido ?
//      SI: seguimos
//      
// 2. Esta Recalibrado ?
//      NO: llamamos a recalibrate
//
// y el proceso continua de la misma forma
//

//int leer_escribir_new(byte operacion, dword sector_logico, byte *buffer)
// Toma todos los argumentos necesarios de la variable global ACTUAL_REQUEST
void leer_escribir_new()
{
  byte cara, cilindro, sector;
  int cmdret;


        if ( getvar("debugfloppy")==1 )
                kprintf("leer_escribir_new: motor=%d calibracion=%d\n",motor_status,floppy_calibrado);  

  // Motor apagado ?
  if (motor_status==MOTOR_OFF) {
                if ( getvar("debugfloppy")==1 )
            kprintf("leer_escribir_new: Motor apagado, encendiendo\n");
    motor_on_new();
    return;
  }

  // Verificamos si existe un timer de apagado
  else if (timer_motor!=NULL) {

                // Debemos eliminarlo
                clean_timer(timer_motor);

    // No existe mas
    timer_motor = NULL;

        }

  if ( ! floppy_calibrado ) {
                //kprintf("leer_escribir_new: Calibrando\n");

                // Comando enviado oks?
                if ( (cmdret=recalibrate_new()) == OK ) {
                 return;
                }

                // Retornamos el error
                else {
                 error();
                 return;
                }

  }


  //Calcular el sector fisico (CHS) a traves del sector logico (LBA)
  cara = (DRIVER_REQUEST_SECTOR(ACTUAL_REQUEST) / SECTORES_POR_PISTA) % CARAS_POR_PISTA;
  cilindro = (DRIVER_REQUEST_SECTOR(ACTUAL_REQUEST) / SECTORES_POR_PISTA) /CARAS_POR_PISTA;
  sector = (DRIVER_REQUEST_SECTOR(ACTUAL_REQUEST) % SECTORES_POR_PISTA) + 1;

        if ( getvar("debugfloppy")==1 )
        kprintf("leer_escribir: sector logico=%d ==> h=%d c=%d s=%d\n", DRIVER_REQUEST_SECTOR(ACTUAL_REQUEST), \
                                                                                                                                                                                                                                                    

  // Esto no deberia ser necesario ya que configuramos el controlador con EIS (Implied Seek) 
  if ( ! seek_status ) {
    //kprintf("leer_escribir_new: Posicionando (Seeking)\n");

                // Comando enviado oks?
                if ( (cmdret=seek_new(cara, cilindro)) == OK )
                        return;

          // Retornamos el error
                else {
                        error();
                        return;
                }
  }

  if ( DRIVER_REQUEST_OPERATION(ACTUAL_REQUEST) == IOREAD ) {

                if ( getvar("debugfloppy")==1 )
                        kprintf("leer_escribir_new: Leyendo\n");

                // Todavia no fue hecha la lectura
    if ( ! rw_status ) {

                        // Comando enviado oks?
                        if ( (cmdret=read_sector_new(cara, cilindro, sector)) == OK )
                                return;

                        // Retornamos el error
                        else {
                                error();
                                return;
                        }


                }

                // Hicimos la lectura en un paso previo, veamos que resulto
                else {
                        //kprintf("leer_escribir_new: Fase de lectura terminada, copiando al buffer\n");

                        word i;

                        byte *dma_block = (byte *) dma_address;
                        byte *buffer = DRIVER_REQUEST_BUFFER(ACTUAL_REQUEST);

                        for(i=0 ; i < BLOQUE_SIZE ; i++) 
                                *buffer++ = *dma_block++;

                        // Oks, ya hicimos todo, resta colocar ok en el retorno del buffer block,       
                        // marcarlo como hecho, despertar a la tarea que genero todo esto y
                        // colocar un timer de apagado de motor
                        floppy_calibrado=0;
                        seek_status=0;
                        rw_status=0;

                        //  Bloque leido correctamente
                        DRIVER_REQUEST_RETURN(ACTUAL_REQUEST) = 1;

                }

        }

        //No implementado
        else if ( DRIVER_REQUEST_OPERATION(ACTUAL_REQUEST) == IOWRITE ) {
                //kprintf("WRITE: operacion no permitida\n");
  }

        // Indicamos al cache que terminamos nuestra operatoria
        endrequest(ACTUAL_REQUEST);

        // Liberamos al floppy
        ACTUAL_REQUEST=NULL;

        // En algunos segundos apagamos el motor
  timer_motor = create_timer(USECONDS_TO_TICKS(TIEMPO_APAGADO_MOTOR_POR_INACTIVIDAD), NULL, motor_off_new, NULL);

  return;

}

// Esta funcion es llamada si se vencio el tiempo de espera de interrupcion
void error_por_timeout()
{

  // El timer ya no existe
  timer_timeout = NULL;  

  // llamamos a la rutina de error
  error();

}

// Algun error, debemos actualizar el bloque indicando dicho error
void error()
{

  // Oks, hubo un error, resta colocarlo en el retorno del buffer block,
  // marcarlo como hecho, despertar a la tarea que genero todo esto y
  // colocar un timer de apagado de motor
  DRIVER_REQUEST_RETURN(ACTUAL_REQUEST) = -1;

  floppy_calibrado=0;
  seek_status=0;
  rw_status=0;

  // Liberamos al floppy
  ACTUAL_REQUEST=NULL;

        // Indicamos al cache que terminamos nuestra operatoria
        endrequest(ACTUAL_REQUEST);

  // En algunos segundos apagamos el motor
  timer_motor = create_timer(USECONDS_TO_TICKS(TIEMPO_APAGADO_MOTOR_POR_INACTIVIDAD), NULL, motor_off_new, NULL);

}

int leer_sec(byte cara, byte cilindro, byte sector , byte *buffer)
{
    if ( read_sector(cara, cilindro , sector)!=OK )
        return ERR_NEED_RESET;

    word i;

    byte *dma_block = (byte *) dma_address;
    for( i=0 ; i < BLOQUE_SIZE ; i++) 
        *buffer++ = *dma_block++;

    return OK;
}

void dump_states()
{
  int posicion;

  for (posicion=0; (posicion<sizeof(status)) && (status[posicion]!=NO_VALIDO); posicion++ )
    dump_status_register(posicion, status[posicion]);


}


static char *ic_codes[] = { "Terminacion Normal", "Terminacion erronea",
                            "Comando invalido"  , "Terminacion erronea causada por polling" };

void dump_status_register(int numero, byte valor)
{
  switch(numero) {

    // ST0 : 7-6:IC, 5:SE, 4:EC, 3:nada, 2:H, 1-0:DS
    case 0:
            //kprintf("ST0: 0x%x - ",valor);
            //kprintf(" IC = 0x%x (%s)\n", (valor&0xc0) >> 6, ic_codes[ (valor&0xc0) >> 6 ]);
            //kprintf(" SE = 0x%x (Seek end %s)\n", (valor&0x20)>>5, ( ( (valor&0x20) >> 5) == 0 ? "erroneo" : "ok") );
            //kprintf(" EC = 0x%x (Equipment Check)\n", (valor&0x10)>>4);
            //kprintf("  H = 0x%x (Head Address)\n", (valor&0x4)>>2);
            //kprintf(" DS = 0x%x (Drive Select)\n", valor&0x3);
            break;

    case 1:
            puts("ST1:");
            //kprintf(" EN = 0x%x (End of cylinder)\n", (valor&0x80)>>7);
            //kprintf(" DE = 0x%x (Data error - CRC)\n", (valor&0x20)>>5);
            //kprintf(" OR = 0x%x (Overrun / Underrun)\n", (valor&0x10)>>4);
            //kprintf(" ND = 0x%x (No Data)\n", (valor&0x4)>>2);
            //kprintf(" NW = 0x%x (Not Writable)\n", (valor&0x2)>>1);
            //kprintf(" MA = 0x%x (Missing Address Mark)\n", valor&0x1);
            break;

    case 2:
            puts("ST2:");
            //kprintf(" CM = 0x%x (Control Mark)\n", (valor&0x40)>>6);
            //kprintf(" DD = 0x%x (Data Error in data field)\n", (valor&0x20)>>5);
            //kprintf(" WC = 0x%x (Wrong Cylinder)\n", (valor&0x10)>>4);
            //kprintf(" BC = 0x%x (Bad cylinder)\n", (valor&0x2)>>1);
            //kprintf(" MD = 0x%x (Missing data address mark)\n", valor&0x1);
            break;

    case 3:
            puts("ST3:");
            //kprintf(" WP = 0x%x (Write Protect)\n", (valor&0x40)>>6);
            //kprintf(" T0 = 0x%x (Track 0)\n", (valor&0x10)>>4);
            //kprintf(" HD = 0x%x (Head Address)\n", (valor&0x4)>>2);
            //kprintf(" DS = 0x%x (Drive Select)\n", valor&0x3);
            break;



  }


}



// Rutinas de atencion de interrupcion

// Bloqueo
void Floppy (void)
{
    // Fin de interrupcion
    endOfInterrupt();

    // Forma vieja
    if ( func == NULL ) {
      floppy_continuar=1;
    }

    // Forma nueva
    else {
      (*func)();
    }

}       


// Procesa los pedidos en el buffer cache
void floppy_procesar_buffer(void)
{

  // Estamos procesando algun otro pedido ?
  if ( ACTUAL_REQUEST != NULL )
    return;


  // Oks, veamos si hay algun pedido para nosotros
  if ( (ACTUAL_REQUEST=getrequest(fd0)) == NULL )
                return;

  //kprintf("\nHay un bloque nuevo para procesar: 0x%x !!!\n",ACTUAL_REQUEST);

  leer_escribir_new();
}



