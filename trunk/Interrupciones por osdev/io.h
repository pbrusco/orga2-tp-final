#ifndef IO
#define IO


/*
 * I/O Functions
 */
__inline__ void outportb(dword port,byte value);
__inline__ void outportw(dword port,dword value);
__inline__ byte inportb(dword port);

#endif //io.h
