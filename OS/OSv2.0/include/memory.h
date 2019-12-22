#ifndef	_MEMORY_H
#define	_MEMORY_H
#include "type.h"
#include "lib.h"

// 一个页页表项个数
#define PTRS_PER_PAGE	512
#define PAGE_OFFSET ((unsigned long)BASE)
#define	PAGE_GDT_SHIFT	39
// 不同模式页表占位
#define	PAGE_1G_SHIFT	30
#define	PAGE_2M_SHIFT	21
#define	PAGE_4K_SHIFT	12
// 屏蔽 2M、4K 以下内存
#define	PAGE_2M_SIZE	(1UL << PAGE_2M_SHIFT)
#define	PAGE_4K_SIZE	(1UL << PAGE_4K_SHIFT)
//	对齐
#define	PAGE_2M_MASK	(~(PAGE_2M_SIZE - 1))
#define	PAGE_4K_MASK	(~(PAGE_4K_SIZE - 1))

#define SIZEOF_LONG_ALIGN(size) ((size + sizeof(long) - 1) & ~(sizeof(long) - 1) )
#define SIZEOF_INT_ALIGN(size) ((size + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define	PAGE_2M_ALIGN(addr)	(((unsigned long)(addr) + PAGE_2M_SIZE - 1) &\
 PAGE_2M_MASK)
#define PAGE_4K_ALIGN(addr)	(((unsigned long)(addr) + PAGE_4K_SIZE - 1) &\
PAGE_4K_MASK)
// 虚拟地址与物理地址的相互转换
#define Virt_To_Phy(addr)	((unsigned long)(addr) - PAGE_OFFSET)
#define Phy_To_Virt(addr)	((unsigned long *)((unsigned long)(addr) + \
PAGE_OFFSET))

#define Virt_To_2M_Page(kaddr)	(memory_management_struct.pages_struct + \
(Virt_To_Phy(kaddr) >> PAGE_2M_SHIFT))
#define Phy_to_2M_Page(kaddr)	(memory_management_struct.pages_struct + \
((unsigned long)(kaddr) >> PAGE_2M_SHIFT))

//// each zone index
static int ZONE_DMA_INDEX;
static int ZONE_NORMAL_INDEX;	//low 1GB RAM ,was mapped in pagetable
static int ZONE_UNMAPED_INDEX;	//above 1GB RAM,unmapped in pagetable

static unsigned long * Global_CR3 = NULL;

#define ZONE_DMA		(1 << 0)
#define ZONE_NORMAL		(1 << 1)
#define ZONE_UNMAPED	(1 << 2)

#define PG_PTable_Maped	(1 << 0)// 映射过的页
#define PG_Kernel_Init	(1 << 1)// 内核初始化程序
// #define PG_Referenced	(1 << 2)// 
// #define PG_Dirty		(1 << 3)
// #define PG_Active		(1 << 4)// 使用中
// #define PG_Up_To_Date	(1 << 5)
#define PG_Device		(1 << 2)// 设备寄存器/内存
#define PG_Kernel		(1 << 3)// 内核层页
#define PG_Shared		(1 << 4)// 共享
// #define PG_K_Share_To_U	(1 << 8)
// #define PG_Slab			(1 << 9)


// 页面
struct Page
{
	struct Zone *	zone_struct;		// 本页所在区域
	unsigned long	PHY_address;		// 映射的物理地址
	unsigned long	attribute;			// 属性

	unsigned long	reference_count;	// 引用次数
	unsigned long	age;				// 创建时间
};
// 内存区域
struct Zone
{
	struct Page * 	pages_group;		// 包含的页面
	unsigned long	pages_length;
	
	unsigned long	zone_start_address;	// 起始页
	unsigned long	zone_end_address;
	unsigned long	zone_length;
	unsigned long	attribute;

	struct Global_Memory_Descriptor * GMD_struct;

	unsigned long	page_using_count;
	unsigned long	page_free_count;

	unsigned long	total_pages_link;	// 物理页被引用次数
};

// packed 属性阻止结构体对齐
struct E820
{
	unsigned long address;
	unsigned long length;
	unsigned int	type;
}__attribute__((packed));

struct Memory_E820_Format
{
	uint32 address1;

	uint32 length1;
	uint32 length2;
	uint32 type;
};

struct Global_Memory_Descriptor
{
	struct E820 	e820[32];
	unsigned long 	e820_length;

	unsigned long * bits_map;		// 物理地址空间页映射图
	unsigned long 	bits_size;		// 物理地址空间页数量
	unsigned long   bits_length;	// 映射位图长度

	struct Page *	pages_struct;	// 指向全局 struct page 结构体数组
	unsigned long	pages_size;		// struct page 结构体总数
	unsigned long 	pages_length;	// 长度

	struct Zone * 	zones_struct;	// 全局 struct zone 结构体数组
	unsigned long	zones_size;		// 数量
	unsigned long 	zones_length;	// 数组长度
					// 内核起始、结束代码		结束数据段地址、结束
	unsigned long 	start_code , end_code , end_data , end_brk;
					// 内存页管理结构的结尾地址
	unsigned long	end_of_struct;	
};
// Slab内存
struct Slab
{
	struct List list;
	struct Page * page;

	unsigned long using_count;
	unsigned long free_count;

	void * Vaddress;

	unsigned long color_length;
	unsigned long color_count;

	unsigned long * color_map;
};
// Slab 池
struct Slab_cache
{
	unsigned long	size;				// 一个 Slab 容纳的字节数
	unsigned long	total_using;
	unsigned long	total_free;
	struct Slab *	cache_pool;// 指向Slab
	struct Slab *	cache_dma_pool;
	void *(* constructor)(void * Vaddress,unsigned long arg);
	void *(* destructor)(void * Vaddress,unsigned long arg);
};
// 定义 16 个不同大小的内存池
extern struct Slab_cache kmalloc_cache_size[16];

extern struct Global_Memory_Descriptor memory_management_struct;

void init_memory();
unsigned long * Get_gdt();
void get_memory_info();
// 页相关函数
struct Page * alloc_pages(int zone_select,int number,unsigned long page_flags);
unsigned long page_init(struct Page * page,unsigned long flags);
unsigned long get_page_attribute(struct Page * page);
// slab 池相关
struct Slab_cache * slab_create(unsigned long size,
	void *(* constructor)(void * Vaddress,unsigned long arg),
	void *(* destructor)(void * Vaddress,unsigned long arg),unsigned long arg);
unsigned long slab_destroy(struct Slab_cache * slab_cache);
void * slab_malloc(struct Slab_cache * slab_cache, unsigned long arg);
unsigned long slab_free(struct Slab_cache * slab_cache,void * address,unsigned long arg);
unsigned long slab_init();
// 内存管理相关
unsigned long kfree(void * address);
void * kmalloc(unsigned long size,unsigned long gfp_flages);
void free_pages(struct Page * page,int number);
unsigned long page_clean(struct Page * page);
struct Slab * kmalloc_create(unsigned long size);


#endif