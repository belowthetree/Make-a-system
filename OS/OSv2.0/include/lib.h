#ifndef _LIB_H
#define _LIB_H

#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE*)0)->MEMBER)

#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (const typeof( ((type *)0)->member ) *)(ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})

// 用于链接各个 PCB
struct List
{
	struct List * prev;
	struct List * next;
};

void wrmsr(unsigned long address,unsigned long value);
void list_init(struct List * list);
struct List * list_next(struct List * entry);

void * memcpy(void *From,void * To,long Num);







#endif