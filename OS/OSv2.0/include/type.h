#ifndef _TYPE_H
#define _TYPE_H

#define NULL	0
#define uint32  unsigned int
#define uint16  unsigned short
#define uint8   unsigned char
#define uint64	unsigned long
#define BASE	0xffff800000000000

// 
typedef struct 
{
	unsigned long pml4t;
} pml4t_t;

#endif