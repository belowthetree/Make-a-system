#include "type.h"
#ifndef	_MEMORY_H
#define	_MEMORY_H

#define PTRS_PER_PAGE	512
#define PAGE_OFFSET ((unsigned long)BASE)
#define	PAGE_GDT_SHIFT	39
#define	PAGE_1G_SHIFT	30
#define	PAGE_2M_SHIFT	21
#define	PAGE_4K_SHIFT	12

#define	PAGE_2M_SIZE	(1UL << PAGE_2M_SHIFT)
#define	PAGE_4K_SIZE	(1UL << PAGE_4K_SHIFT)

#define	PAGE_2M_MASK	(~	(PAGE_2M_SIZE - 1))
#define	PAGE_4K_SIZE	(~	(PAGE_4K_SIZE - 1))

#define	PAGE_2M_ALIGN(addr)	(((unsigned long)(addr) + PAGE_2M_SIZE - 1) &\
 PAGE_2M_MASK)
#define PAGE_4K_ALIGN(addr)	(((unsigned long)(addr) + PAGE_4K_SIZE - 1) &\
PAGE_4K_MASK)

#define Virt_To_Phy(addr)	((unsigned long)(addr) - PAGE_OFFSET)
#define Phy_To_Virt(addr)	((unsigned long *)((unsigned long)(addr) + \
PAGE_OFFSET))

#define Virt_To_2M_Page(kaddr)	(memory_management_struct.pages_struct + \
(Virt_To_Phy(kaddr) >> PAGE_2M_SHIFT))
#define Phy_to_2M_Page(kaddr)	(memory_management_struct.pages_struct + \
((unsigned long)(kaddr) >> PAGE_2M_SHIFT))


struct Memory_E820_Format
{
	uint32 address1;
	uint32 address2;
	uint32 length1;
	uint32 length2;
	uint32 type;
};

void init_memory();












#endif