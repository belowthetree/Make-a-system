#include "lib.h"
#include "type.h"

// 将 EDX:EAX 里的值写入 ECX指示的 MSR 寄存器中
void wrmsr(unsigned long address,unsigned long value)
{
	__asm__ __volatile__("wrmsr	\n\t"::"d"(value >> 32),"a"(value & 0xffffffff),"c"(address):"memory");	
}

void list_init(struct List * list)
{
	list->prev = list;
	list->next = list;
}

void list_add_to_behind(struct List * entry,struct List * new)	////add to entry behind
{
	new->next = entry->next;
	new->prev = entry;
	new->next->prev = new;
	entry->next = new;
}

void list_add_to_before(struct List * entry,struct List * new)	////add to entry behind
{
	new->next = entry;
	entry->prev->next = new;
	new->prev = entry->prev;
	entry->prev = new;
}

void list_del(struct List * entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
}

long list_is_empty(struct List * entry)
{
	if(entry == entry->next && entry->prev == entry)
		return 1;
	else
		return 0;
}

struct List * list_prev(struct List * entry)
{
	if(entry->prev != NULL)
		return entry->prev;
	else
		return NULL;
}

struct List * list_next(struct List * entry)
{
	if(entry->next != NULL)
		return entry->next;
	else
		return NULL;
}

void * memcpy(void *From,void * To,long Num)
{
	int d0,d1,d2;
	__asm__ __volatile__	(	"cld	\n\t"
					"rep	\n\t"
					"movsq	\n\t"
					"testb	$4,%b4	\n\t"
					"je	1f	\n\t"
					"movsl	\n\t"
					"1:\ttestb	$2,%b4	\n\t"
					"je	2f	\n\t"
					"movsw	\n\t"
					"2:\ttestb	$1,%b4	\n\t"
					"je	3f	\n\t"
					"movsb	\n\t"
					"3:	\n\t"
					:"=&c"(d0),"=&D"(d1),"=&S"(d2)
					:"0"(Num/8),"q"(Num),"1"(To),"2"(From)
					:"memory"
				);
	return To;
}