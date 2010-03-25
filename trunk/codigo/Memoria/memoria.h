#ifndef __MEMORIA__
#define __MEMORIA__

#define DIRECCION_BITMAP 0x00100000


void llenarBitmap();


void* malloc(int cant_bytes);

id_tarea obtenerIdActual();
header* obtenerDirTabla(id_tarea id_actual);
void* obtenerDirDisponible(header* tabla,unsigned int cant_bytes);

#endif
