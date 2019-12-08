#ifndef _LIB_H
#define _LIB_H

#define container_of(ptr,type,member)							\
({											\
	typeof(((type *)0)->member) * p = (ptr);					\
	(type *)((unsigned long)p - (unsigned long)&(((type *)0)->member));		\
})

// 用于链接各个 PCB
struct List
{
	struct List * prev;
	struct List * next;
};

void wrmsr(unsigned long address,unsigned long value);
void list_init(struct List * list);
struct List * list_next(struct List * entry);








#endif