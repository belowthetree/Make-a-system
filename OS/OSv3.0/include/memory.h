#ifndef MEMORY_H
#define MEMORY_H

#define	PAGE_2M_SHIFT	21
#define	PAGE_2M_SIZE	(1UL << PAGE_2M_SHIFT)
#define	PAGE_4K_SIZE	(1UL << PAGE_4K_SHIFT)
#define	PAGE_2M_MASK	(~(PAGE_2M_SIZE - 1))
#define	PAGE_4K_MASK	(~(PAGE_4K_SIZE - 1))
#define PAGE_OFFSET 	((unsigned long)BASE)

#define AREABASE 0xffff800000000000
#define MAPBASE  0xffff800000000100
#define AREASIZE 10

int AREANUM;

struct MEMORY_E820{
	unsigned long addr;
	unsigned long len;
	unsigned int type;
}__attribute__((packed));

struct PAGE{
	unsigned long virtual_addr;
	unsigned long physical_addr;
	unsigned long attr;
};

struct SLAB{
	struct PAGE* page;
	unsigned long free_count;
	unsigned long used_count;
	unsigned long map_len;
	unsigned char* bitmap;
	struct SLAB* prev, *next;
};

struct SLAB_CACHE{
	struct SLAB* cache_pool;
	unsigned long free_slab;
	unsigned long used_slab;
	int size;
}Slab_cache[16];

struct AREA{
	unsigned long physical_addr;
	unsigned long virtual_addr;
	unsigned long free_page;
	unsigned long end;
};

struct MEMORY_MGR{
	struct AREA *area[AREASIZE];
	struct PAGE* pages;
	unsigned long free_page;
	unsigned long used_page;
	unsigned char* bitmap;
	unsigned long kernel_start;
	unsigned long kernel_end;
	int map_len;
	int total_page;
	int total_slab;
}memory_manager;



void InitMemory();
unsigned long kmalloc(int size);
void kfree(unsigned long addr);










#define	PAGE_2M_ALIGN(addr)	(((unsigned long)(addr) + PAGE_2M_SIZE - 1) &\
 PAGE_2M_MASK)

// 虚拟地址与物理地址的相互转换
#define Virt_To_Phy(addr)	((unsigned long)(addr) - PAGE_OFFSET)
#define Phy_To_Virt(addr)	((unsigned long)((unsigned long)(addr) + \
PAGE_OFFSET))

////页表属性
//	bit 63	Execution Disable:
#define PAGE_XD		(unsigned long)0x1000000000000000
//	bit 12	Page Attribute Table
#define	PAGE_PAT	(unsigned long)0x1000
//	bit 8	Global Page:1,global;0,part
#define	PAGE_Global	(unsigned long)0x0100
//	bit 7	Page Size:1,big page;0,small page;
#define	PAGE_PS		(unsigned long)0x0080
//	bit 6	Dirty:1,dirty;0,clean;
#define	PAGE_Dirty	(unsigned long)0x0040
//	bit 5	Accessed:1,visited;0,unvisited;
#define	PAGE_Accessed	(unsigned long)0x0020
//	bit 4	Page Level Cache Disable
#define PAGE_PCD	(unsigned long)0x0010
//	bit 3	Page Level Write Through
#define PAGE_PWT	(unsigned long)0x0008
//	bit 2	User Supervisor:1,user and supervisor;0,supervisor;
#define	PAGE_U_S	(unsigned long)0x0004
//	bit 1	Read Write:1,read and write;0,read;
#define	PAGE_R_W	(unsigned long)0x0002
//	bit 0	Present:1,present;0,no present;
#define	PAGE_Present	(unsigned long)0x0001

//1,0
#define PAGE_KERNEL_GDT		(PAGE_R_W | PAGE_Present)
//1,0	
#define PAGE_KERNEL_DIR		(PAGE_R_W | PAGE_Present)
//7,1,0
#define	PAGE_KERNEL_PAGE	(PAGE_PS  | PAGE_R_W | PAGE_Present)
//2,1,0
#define PAGE_USER_DIR		(PAGE_U_S | PAGE_R_W | PAGE_Present)
//7,2,1,0
#define	PAGE_USER_PAGE		(PAGE_PS  | PAGE_U_S | PAGE_R_W | PAGE_Present)

















#endif