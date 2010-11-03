#include "k_defines.h"


/*
 * Simply clears the screen to ' '
 */
void clear_screen()
{
	char *vidmem = (char *)0xB8000;
	unsigned int i = 0;

	while(i < (80*25*2))
	{
    vidmem[i] = ' ';
    i++;
	vidmem[i] = 0x07;
	i++;
	}
}

/*
 * Print only supports a-z and 0-9 and other keyboard characters
 */
void print(char *msg, unsigned int line)
{
	char *vidmem = (char *)0xB8000;
    unsigned int i = line*80*2, color = WHITE_TXT;

		
	
	while(*msg != 0)
	{ 		  	
		  vidmem[i] = *msg;
		  i++;
		  vidmem[i] = color;
		  i++;
		  *msg++;
	}
}
