#ifndef LIB_H
#define LIB_H

#define sti() 		__asm__ __volatile__ ("sti	\n\t":::"memory")


void io_out8(unsigned short port,unsigned char value);
void io_out32(unsigned short port,unsigned int value);

unsigned char io_in8(unsigned short port);
unsigned int io_in32(unsigned short port);

#define load_TR(n) 							\
do{									\
	__asm__ __volatile__(	"ltr	%%ax"				\
				:					\
				:"a"(n << 3)				\
				:"memory");				\
}while(0)

#define load_gdtr(limit, addr)\
do{\
	__asm__ __volatile__(\
		"movw %%ax, (%%rsp)\n\t"\
		"movq %%rbx, 2(%%rsp)\n\t"\
		"lgdt (%%rsp)"\
		:\
		:"a"(limit)\
		,"b"(addr)\
		:"memory");\
}while(0)

#define load_idtr(limit, addr)\
do{\
	__asm__ __volatile__(\
		"movw %%ax, (%%rsp)\n\t"\
		"movq %%rbx, 2(%%rsp)\n\t"\
		"lidt (%%rsp)"\
		:\
		:"a"(limit)\
		,"b"(addr)\
		:"memory");\
}while(0)






















#endif