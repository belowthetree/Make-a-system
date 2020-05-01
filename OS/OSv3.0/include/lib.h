#ifndef LIB_H
#define LIB_H

#define sti() 		__asm__ __volatile__ ("sti	\n\t":::"memory")


void io_out8(unsigned short port,unsigned char value);
void io_out32(unsigned short port,unsigned int value);

unsigned char io_in8(unsigned short port);
unsigned int io_in32(unsigned short port);
























#endif