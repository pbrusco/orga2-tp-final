#ifndef __MEMORIA__
#define __MEMORIA__

#define DIRECCION_BITMAP 0x00100000
#define DIRECCION_BASE_MALLOC 0XE7000000

void llenarBitmap();



void* malloc(unsigned int cant_bytes);

id_tarea obtenerIdActual();
header* obtenerDirTabla(id_tarea id_actual);
void* obtenerDirDisponible(header* tabla,unsigned int cant_bytes);
void* devolverDondeEntre(header* tabla,unsigned int cant_bytes);

#endif
