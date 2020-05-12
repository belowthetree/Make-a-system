#ifndef _LIB_H
#define _LIB_H
#include "type.h"

#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE*)0)->MEMBER)

#define io_mfence() __asm__ ("mfence")

#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (const typeof( ((type *)0)->member ) *)(ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define nop() 		__asm__ __volatile__ ("nop	\n\t")

#define port_insw(port,buffer,nr)	\
__asm__ __volatile__("cld;rep;insw;mfence;"::"d"(port),"D"(buffer),"c"(nr):"memory")

// 用于链接各个 PCB
struct List
{
	struct List * prev;
	struct List * next;
};

void wrmsr(unsigned long address,unsigned long value);
void list_init(struct List * list);
struct List * list_next(struct List * entry);

inline long list_is_empty(struct List * entry)
{
	if(entry == entry->next && entry->prev == entry)
		return 1;
	else
		return 0;
}

inline void list_del(struct List * entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
}

inline struct List * list_prev(struct List * entry)
{
	if(entry->prev != NULL)
		return entry->prev;
	else
		return NULL;
}

inline void list_add_to_behind(struct List * entry,struct List * new)	////add to entry behind
{
	new->next = entry->next;
	new->prev = entry;
	new->next->prev = new;
	entry->next = new;
}

inline void list_add_to_before(struct List * entry,struct List * new)	////add to entry behind
{
	new->next = entry;
	entry->prev->next = new;
	new->prev = entry->prev;
	entry->prev = new;
}



void * memcpy(void *From,void * To,long Num);







#endif