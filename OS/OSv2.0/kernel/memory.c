#include "memory.h"
#include "type.h"
#include "graph.h"
#include "io.h"

#define SIZEOF_LONG_ALIGN(size) ((size + sizeof(long) - 1) & ~(sizeof(long) - 1) )
#define SIZEOF_INT_ALIGN(size) ((size + sizeof(int) - 1) & ~(sizeof(int) - 1) )

struct Slab_cache kmalloc_cache_size[16];

extern char _text;
extern char _etext;
extern char _edata;
extern char _end;

#define flush_tlb()						\
do								\
{								\
	unsigned long	tmpreg;					\
	__asm__ __volatile__ 	(				\
				"movq	%%cr3,	%0	\n\t"	\
				"movq	%0,	%%cr3	\n\t"	\
				:"=r"(tmpreg)			\
				:				\
				:"memory"			\
				);				\
}while(0)

void init_memory()
{
	memory_management_struct.start_code = (uint64) & _text;
	memory_management_struct.end_code 	= (uint64) & _etext;
	memory_management_struct.end_data 	= (uint64) & _edata;
	memory_management_struct.end_brk 	= (uint64) & _end;

	int i,j;
	unsigned long TotalMem = 0;
	struct E820 *p = NULL;
	
	printf_color(BLACK, BLUE, "Display Physics Address MAP,Type(1:RAM,2:ROM or Reserved,3:ACPI Reclaim Memory,4:ACPI NVS Memory,Others:Undefine)\n");
	p = (struct E820 *)0xffff800000007e00;

	for(i = 0;i < 32;i++)
	{
		printf_color(BLACK, ORANGE, "Address:%10uX\t\
			Length:%10uX\t Type:%10uX\n",
			p->address, p->length, p->type);
		unsigned long tmp = 0;
		if(p->type == 1)
			TotalMem +=  p->length;

		memory_management_struct.e820[i].address = p->address;
		memory_management_struct.e820[i].length	 = p->length;
		memory_management_struct.e820[i].type	 = p->type;
		memory_management_struct.e820_length 	 = i;

		p++;
		if(p->type > 4 || p->length == 0 || p->type < 1)
			break;
	}

	printf_color(BLACK, ORANGE, "OS Can Use Total RAM:%X\n",TotalMem);

	TotalMem = 0;
	// 计算总内存
	for(i = 0;i <= memory_management_struct.e820_length;i++)
	{
		unsigned long start,end;
		if(memory_management_struct.e820[i].type != 1)
			continue;
		start = PAGE_2M_ALIGN(memory_management_struct.e820[i].address);
		end   = ((memory_management_struct.e820[i].address + memory_management_struct.e820[i].length) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;
		if(end <= start)
			continue;
		TotalMem += (end - start) >> PAGE_2M_SHIFT;
	}
	printf_color(BLACK, ORANGE, "OS Can Used Total 2M PAGEs:%X=%ld\n",TotalMem,TotalMem);

	TotalMem = memory_management_struct.e820[memory_management_struct.e820_length].
	address + memory_management_struct.e820[memory_management_struct.e820_length].length;

	/*位图初始化*/
	// 指向可用内存的最末尾
	memory_management_struct.bits_map = (unsigned long *)((memory_management_struct.end_brk +
	 	PAGE_4K_SIZE - 1) & PAGE_4K_MASK);

	// 页数
	memory_management_struct.bits_size = TotalMem >> PAGE_2M_SHIFT;
	// 数组元素个数（或者占用内存）
	memory_management_struct.bits_length = (((unsigned long)(TotalMem >> 
		PAGE_2M_SHIFT) + sizeof(long) * 8 - 1) / 8) & ( ~ (sizeof(long) - 1));
	memset((unsigned char*)memory_management_struct.bits_map,
		0xff,memory_management_struct.bits_length);		//init bits map memory

	/*页初始化*/
	// 页数组放在位图后
	memory_management_struct.pages_struct = (struct Page *)(((unsigned long)
		memory_management_struct.bits_map + memory_management_struct.bits_length
		 + PAGE_4K_SIZE - 1) & PAGE_4K_MASK);
	// 页数
	memory_management_struct.pages_size = TotalMem >> PAGE_2M_SHIFT;
	// 页表占用内存（单位 long）
	memory_management_struct.pages_length = ((TotalMem >> PAGE_2M_SHIFT)
	 	* sizeof(struct Page) + sizeof(long) - 1) & ( ~ (sizeof(long) - 1));
	memset((unsigned char*)memory_management_struct.pages_struct,
		0x00,memory_management_struct.pages_length);	//init pages memory

	/*区域初始化*/
	// 放在区域后面
	memory_management_struct.zones_struct = (struct Zone *)(((unsigned long)
		memory_management_struct.pages_struct +
		memory_management_struct.pages_length + PAGE_4K_SIZE - 1) & PAGE_4K_MASK);
	memory_management_struct.zones_size   = 0;
	memory_management_struct.zones_length = (5 * sizeof(struct Zone) + 
		sizeof(long) - 1) & (~(sizeof(long) - 1));
	memset((unsigned char*)memory_management_struct.zones_struct,0x00,memory_management_struct.zones_length);	//init zones memory

	for(i = 0;i <= memory_management_struct.e820_length;i++)
	{
		unsigned long start,end;
		struct Zone * z;
		struct Page * p;
		unsigned long * b;

		if(memory_management_struct.e820[i].type != 1)
			continue;
		// 将BIOS给出的不同区域内存赋值给 zone
		start = PAGE_2M_ALIGN(memory_management_struct.e820[i].address);
		end   = ((memory_management_struct.e820[i].address + memory_management_struct.e820[i].length) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;
		if(end <= start)
			continue;

		//zone init
		z = memory_management_struct.zones_struct + 
			memory_management_struct.zones_size++;

		z->zone_start_address = start;
		z->zone_end_address = end;
		z->zone_length = end - start;
		// 使用次数、空闲页面
		z->page_using_count = 0;
		z->page_free_count = (end - start) >> PAGE_2M_SHIFT;
		// 页面引用次数
		z->total_pages_link = 0;
		// 属性、全局内存管理段指针 Global_Memory_Descriptor
		z->attribute = 0;
		z->GMD_struct = &memory_management_struct;
		// 对应全局内存结构中的页面数组的局部
		z->pages_length = (end - start) >> PAGE_2M_SHIFT;
		z->pages_group =  (struct Page *)(memory_management_struct.pages_struct + (start >> PAGE_2M_SHIFT));
		//page init
		p = z->pages_group;
		for(j = 0;j < z->pages_length; j++ , p++)
		{
			p->zone_struct = z;
			p->PHY_address = start + PAGE_2M_SIZE * j;
			p->attribute = 0;

			p->reference_count = 0;
			p->age = 0;
			// 一个位图元素 8 字节，对应 64 张页，一张页对应一个位，空闲为0
			*(memory_management_struct.bits_map + 
				((p->PHY_address >> PAGE_2M_SHIFT) >> 6)) ^= 
			1UL << (p->PHY_address >> PAGE_2M_SHIFT) % 64;
		}
	}
	// 初始 2M 处页表初始化，归第一个区域
	memory_management_struct.pages_struct->zone_struct = memory_management_struct.zones_struct;

	memory_management_struct.pages_struct->PHY_address = 0UL;
	memory_management_struct.pages_struct->attribute = 0;
	memory_management_struct.pages_struct->reference_count = 0;
	memory_management_struct.pages_struct->age = 0;

	memory_management_struct.zones_length = 
		(memory_management_struct.zones_size * sizeof(struct Zone) + 
			sizeof(long) - 1) & ( ~ (sizeof(long) - 1));

	printf_color(BLACK, ORANGE, "bits_map:\t%18uX,bits_size:\t%18uX,bits_length:\t%18uX\n",memory_management_struct.bits_map,memory_management_struct.bits_size,memory_management_struct.bits_length);
	printf_color(BLACK, ORANGE, "pages_struct:%18uX,pages_size:\t%18uX,pages_length:\t%18uX\n",memory_management_struct.pages_struct,memory_management_struct.pages_size,memory_management_struct.pages_length);
	printf_color(BLACK, ORANGE, "zones_struct:%18uX,zones_size:\t%18uX,zones_length:\t%18uX\n",memory_management_struct.zones_struct,memory_management_struct.zones_size,memory_management_struct.zones_length);

	ZONE_DMA_INDEX = 0;	//need rewrite in the future
	ZONE_NORMAL_INDEX = 0;	//need rewrite in the future

	for(i = 0;i < memory_management_struct.zones_size;i++)	//need rewrite in the future
	{
		struct Zone * z = memory_management_struct.zones_struct + i;
		printf_color(BLACK, ORANGE, "zone_start_address:%18uX,zone_end_address:%18uX,zone_length:%18uX,pages_group:%18uX,pages_length:%18uX\n",z->zone_start_address,z->zone_end_address,z->zone_length,z->pages_group,z->pages_length);
		// 4G 以后的区域设置为未映射
		if(z->zone_start_address == 0x100000000)
			ZONE_UNMAPED_INDEX = i;
	}
	// 全局内存管理结构体的结尾
	memory_management_struct.end_of_struct = 
		(unsigned long)((unsigned long)memory_management_struct.zones_struct + 
		memory_management_struct.zones_length + 
		sizeof(long) * 32) & ( ~ (sizeof(long) - 1));	////need a blank to separate memory_management_struct

	printf_color(BLACK, ORANGE, "start_code:%018X,end_code:%018X,end_data:%018X,end_brk:%018X,end_of_struct:%018X\n",memory_management_struct.start_code,memory_management_struct.end_code,memory_management_struct.end_data,memory_management_struct.end_brk, memory_management_struct.end_of_struct);

	i = Virt_To_Phy(memory_management_struct.end_of_struct) >> PAGE_2M_SHIFT;
	// 初始化页表
	for(j = 0;j <= i;j++)
	{
		page_init(memory_management_struct.pages_struct + 
			j,PG_PTable_Maped | PG_Kernel_Init | PG_Active | PG_Kernel);
	}

	Global_CR3 = Get_gdt();

	printf_color(BLACK, INDIGO, "Global_CR3\t:%018X\n",Global_CR3);
	printf_color(BLACK, INDIGO, "*Global_CR3\t:%018X\n",*Phy_To_Virt(Global_CR3) & (~0xff));
	printf_color(BLACK, INDIGO, "**Global_CR3\t:%018X\n",*Phy_To_Virt(*Phy_To_Virt(Global_CR3) & (~0xff)) & (~0xff));
	// 清除页表，作者说法是不需要保留一致性页表映射，可能是为了避免访问内核？或者重新分配
	//for(i = 0;i < 10;i++)
	//	*(Phy_To_Virt(Global_CR3)  + i) = 0UL;
	
	flush_tlb();
}

unsigned long page_init(struct Page * page,unsigned long flags)
{
	page->attribute |= flags;
	
	if(!page->reference_count || (page->attribute & PG_Shared))
	{
		page->reference_count++;
		page->zone_struct->total_pages_link++;		
	}	
	
	return 1;
}

unsigned long * Get_gdt()
{
	unsigned long * tmp;
	__asm__ __volatile__	(
					"movq	%%cr3,	%0	\n\t"
					:"=r"(tmp)
					:
					:"memory"
				);
	return tmp;
}

/*
	number: number < 64
	zone_select: zone select from dma , mapped in  pagetable , unmapped in pagetable
	page_flags: struct Page flages
*/
struct Page * alloc_pages(int zone_select,int number,unsigned long page_flags)
{
	int i;
	unsigned long page = 0;

	int zone_start = 0;
	int zone_end = 0;
	// 根据类型选择不同区域，不过目前没分类，都一样
	switch(zone_select)
	{
		case ZONE_DMA:
				zone_start = 0;
				zone_end = ZONE_DMA_INDEX;
			break;

		case ZONE_NORMAL:
				zone_start = ZONE_DMA_INDEX;
				zone_end = ZONE_NORMAL_INDEX;
			break;

		case ZONE_UNMAPED:
				zone_start = ZONE_UNMAPED_INDEX;
				zone_end = memory_management_struct.zones_size - 1;
			break;

		default:
			printf_color(BLACK, RED, "alloc_pages error zone_select index\n");
			return NULL;
			break;
	}

	// 从选定区域开始寻找
	for(i = zone_start;i <= zone_end; i++)
	{
		struct Zone * z;
		unsigned long j;
		unsigned long start,end,length;
		unsigned long tmp;
		// 当前区域空闲页数少于需要的页，则跳过
		if((memory_management_struct.zones_struct + i)->page_free_count < number)
			continue;
		// start、end 分别为开始、结束的页表
		z = memory_management_struct.zones_struct + i;
		start = z->zone_start_address >> PAGE_2M_SHIFT;
		end = z->zone_end_address >> PAGE_2M_SHIFT;
		length = z->zone_length >> PAGE_2M_SHIFT;

		tmp = 64 - start % 64;
		// 这里先将 j 补齐到对齐 64 位
		for(j = start;j <= end;j += j % 64 ? tmp : 64)
		{	// 得到第 j 个页表空闲情况位图地址
			unsigned long * p = memory_management_struct.bits_map + (j >> 6);
			unsigned long shift = j % 64;
			unsigned long k;
			for(k = shift;k < 64 - shift;k++)
			{	// 判断是否有从 p 开始的 number 个页表可以使用
				if( !(((*p >> k) | (*(p + 1) << (64 - k))) & 
					(number == 64 ? 0xffffffffffffffffUL : ((1UL << number) - 1))))
				{
					unsigned long	l;
					// 可能要改
					page = j / 64 * 64 + k;
					for(l = 0;l < number;l++)
					{
						struct Page * x = memory_management_struct.pages_struct + page + l;
						page_init(x,page_flags);
					}
					goto find_free_pages;
				}
			}
		
		}
	}

	return NULL;

find_free_pages:

	return (struct Page *)(memory_management_struct.pages_struct + page);
}
// 创建 slab_cache
struct Slab_cache * slab_create(unsigned long size,
	void *(* constructor)(void * Vaddress,unsigned long arg),
	void *(* destructor)(void * Vaddress,unsigned long arg),unsigned long arg)
{
	struct Slab_cache * slab_cache = NULL;

	slab_cache = (struct Slab_cache *)kmalloc(sizeof(struct Slab_cache),0);
	
	if(slab_cache == NULL)
	{
		printf_color(BLACK, RED, "slab_create()->kmalloc()=>slab_cache == NULL\n");
		return NULL;
	}
	// 申请成功之后需要先清空
	memset(slab_cache,0,sizeof(struct Slab_cache));
	// 对齐，初始化，先创建一个 slab
	slab_cache->size = SIZEOF_LONG_ALIGN(size);
	slab_cache->total_using = 0;
	slab_cache->total_free = 0;
	slab_cache->cache_pool = (struct Slab *)kmalloc(sizeof(struct Slab),0);
	
	if(slab_cache->cache_pool == NULL)
	{
		printf_color(BLACK, RED, "slab_create()->kmalloc()=>slab_cache->cache_pool == NULL\n");
		kfree(slab_cache);
		return NULL;
	}
	
	memset(slab_cache->cache_pool,0,sizeof(struct Slab));
	// 构造、析构函数
	slab_cache->cache_dma_pool = NULL;
	slab_cache->constructor = constructor;
	slab_cache->destructor = destructor;

	list_init(&slab_cache->cache_pool->list);
	// 为第一个 slab 申请页
	slab_cache->cache_pool->page = alloc_pages(ZONE_NORMAL,1,0);
	
	if(slab_cache->cache_pool->page == NULL)
	{
		printf_color(BLACK, RED, "slab_create()->alloc_pages()=>slab_cache->cache_pool->page == NULL\n");
		kfree(slab_cache->cache_pool);
		kfree(slab_cache);
		return NULL;
	}
	
	page_init(slab_cache->cache_pool->page,PG_Kernel);

	slab_cache->cache_pool->using_count = PAGE_2M_SIZE/slab_cache->size;
	slab_cache->cache_pool->free_count = slab_cache->cache_pool->using_count;
	slab_cache->total_free = slab_cache->cache_pool->free_count;

	slab_cache->cache_pool->Vaddress = Phy_To_Virt(slab_cache->cache_pool->page->PHY_address);

	slab_cache->cache_pool->color_count = slab_cache->cache_pool->free_count;
	slab_cache->cache_pool->color_length = ((slab_cache->cache_pool->color_count + sizeof(unsigned long) * 8 - 1) >> 6) << 3;
	slab_cache->cache_pool->color_map = (unsigned long *)kmalloc(slab_cache->cache_pool->color_length,0);

	if(slab_cache->cache_pool->color_map == NULL)
	{
		printf_color(BLACK, RED, "slab_create()->kmalloc()=>slab_cache->cache_pool->color_map == NULL\n");
		
		free_pages(slab_cache->cache_pool->page,1);
		kfree(slab_cache->cache_pool);
		kfree(slab_cache);
		return NULL;
	}
	
	memset(slab_cache->cache_pool->color_map,0,slab_cache->cache_pool->color_length);

	return slab_cache;
}
// slab_cache 析构
unsigned long slab_destroy(struct Slab_cache * slab_cache)
{
	struct Slab * slab_p = slab_cache->cache_pool;
	struct Slab * tmp_slab = NULL;

	if (slab_cache->total_using != 0)
	{
		printf_color(BLACK, RED, "slab_cache->total_using != 0\n");
		return 0;
	}

	while(!list_is_empty(&slab_p->list))
	{
		tmp_slab = slab_p;
		slab_p = container_of(list_next(&slab_p->list), struct Slab, list);

		list_del (&tmp_slab->list);
		kfree(tmp_slab->color_map);

		page_clean(tmp_slab->page);
		free_pages(tmp_slab->page, 1);
		kfree(tmp_slab);
	}

	kfree(slab_p->color_map);
	page_clean(slab_p->page);
	free_pages(slab_p->page, 1);
	kfree(slab_p);
	kfree(slab_cache);
	return 1;
}
// 申请 slab
void * slab_malloc(struct Slab_cache * slab_cache, unsigned long arg)
{
	struct Slab * slab_p = slab_cache->cache_pool;
	struct Slab * tmp_slab = NULL;
	int j = 0;
	// 如果没有空余 slab，进行内存申请
	if (slab_cache->total_free == 0)
	{
		tmp_slab = (struct Slab *) kmalloc(sizeof(struct Slab), 0);
		// 如果没有申请到，返回空
		if (tmp_slab == NULL)
		{
			printf_color(BLACK, RED, "slab_malloc()->kmalloc()=>tmp_slab == NULL\n");
			return NULL;
		}
		// 初始化
		memset(tmp_slab, 0, sizeof(struct Slab));
		list_init(&tmp_slab->list);
		// 申请页，失败释放内存，并返回空
		tmp_slab->page = alloc_pages(ZONE_NORMAL, 1, 0);
		if (tmp_slab->page == NULL)
		{
			printf_color(BLACK, RED, "slab_malloc()->alloc_pages()=>tmp_slab->page == NULL\n");
			kfree(tmp_slab);
			return NULL;
		}

		page_init(tmp_slab->page, PG_Kernel);
		// 容量为一个页表，设置虚拟地址，位图数量根据缓冲池 size 确定
		tmp_slab->using_count = PAGE_2M_SIZE / slab_cache->size;
		tmp_slab->free_count = tmp_slab->using_count;
		tmp_slab->Vaddress = Phy_To_Virt(tmp_slab->page->PHY_address);
		// 位图对应
		tmp_slab->color_count = tmp_slab->free_count;
		tmp_slab->color_length = ((tmp_slab->color_count + sizeof(unsigned long) * 8 - 1) >> 6) << 3;
		tmp_slab->color_map = (unsigned long *)kmalloc(tmp_slab->color_length,0);
		// 位图申请失败
		if(tmp_slab->color_map == NULL)
		{
			printf_color(BLACK, RED, "slab_malloc()->kmalloc()=>tmp_slab->color_map == NULL\n");
			free_pages(tmp_slab->page,1);
			kfree(tmp_slab);
			return NULL;
		}
		// 初始化，链接
		memset(tmp_slab->color_map,0,tmp_slab->color_length);	
		list_add_to_behind(&slab_cache->cache_pool->list,&tmp_slab->list);
		slab_cache->total_free  += tmp_slab->color_count;	

		for(j = 0;j < tmp_slab->color_count;j++)
		{
			if( (*(tmp_slab->color_map + (j >> 6)) & (1UL << (j % 64))) == 0 )
			{
				*(tmp_slab->color_map + (j >> 6)) |= 1UL << (j % 64);
			
				tmp_slab->using_count++;
				tmp_slab->free_count--;

				slab_cache->total_using++;
				slab_cache->total_free--;
				
				if(slab_cache->constructor != NULL)
				{
					return slab_cache->constructor((char *)tmp_slab->Vaddress + slab_cache->size * j,arg);
				}
				else
				{			
					return (void *)((char *)tmp_slab->Vaddress + slab_cache->size * j);
				}		
			}
		}
	}
	else
	{
		do
		{
			if(slab_p->free_count == 0)
			{
				slab_p = container_of(list_next(&slab_p->list),struct Slab,list);
				continue;
			}

			for(j = 0;j < slab_p->color_count;j++)
			{
				// 全 1 则跳转
				if(*(slab_p->color_map + (j >> 6)) == 0xffffffffffffffffUL)
				{
					j += 63;
					continue;
				}	
				// 找到空位
				if( (*(slab_p->color_map + (j >> 6)) & (1UL << (j % 64))) == 0 )
				{
					*(slab_p->color_map + (j >> 6)) |= 1UL << (j % 64);
				
					slab_p->using_count++;
					slab_p->free_count--;

					slab_cache->total_using++;
					slab_cache->total_free--;
					
					if(slab_cache->constructor != NULL)
						return slab_cache->constructor((char *)slab_p->Vaddress + 
							slab_cache->size * j,arg);
					else
						return (void *)((char *)slab_p->Vaddress + slab_cache->size * j);
				}
			}
		}while(slab_p != slab_cache->cache_pool);		
	}
	// 如果还在第一个 Slab，说明有问题
	printf_color(BLACK, RED, "slab_malloc() ERROR: can`t alloc\n");
	if(tmp_slab != NULL)
	{
		list_del(&tmp_slab->list);
		kfree(tmp_slab->color_map);
		page_clean(tmp_slab->page);
		free_pages(tmp_slab->page,1);
		kfree(tmp_slab);
	}

	return NULL;
}
// 释放 slab
unsigned long slab_free(struct Slab_cache * slab_cache,void * address,unsigned long arg)
{
	struct Slab * slab_p = slab_cache->cache_pool;
	int index = 0;

	do
	{
		if(slab_p->Vaddress <= address && address < slab_p->Vaddress + PAGE_2M_SIZE)
		{	// 将对应内存的位图清空
			index = (address - slab_p->Vaddress) / slab_cache->size;
			*(slab_p->color_map + (index >> 6)) ^= 1UL << index % 64;
			slab_p->free_count++;
			slab_p->using_count--;

			slab_cache->total_using--;
			slab_cache->total_free++;
			// 如果存在析构函数
			if(slab_cache->destructor != NULL)
			{
				slab_cache->destructor((char *)slab_p->Vaddress + slab_cache->size * index,arg);
			}
			// 如果
			if((slab_p->using_count == 0) && (slab_cache->total_free >= slab_p->color_count * 3 / 2))
			{
				list_del(&slab_p->list);
				slab_cache->total_free -= slab_p->color_count;

				kfree(slab_p->color_map);
				
				page_clean(slab_p->page);
				free_pages(slab_p->page,1);
				kfree(slab_p);				
			}

			return 1;
		}
		else
		{
			slab_p = container_of(list_next(&slab_p->list),struct Slab,list);
			continue;	
		}		

	}while(slab_p != slab_cache->cache_pool);

	printf_color(BLACK, RED, "slab_free() ERROR: address not in slab\n");

	return 0;
}
// 为 slab_cache 分配空间
unsigned long slab_init()
{
	struct Slab_cache t[16] = 
{
	{32	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{64	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{128	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{256	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{512	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{1024	,0	,0	,NULL	,NULL	,NULL	,NULL},			//1KB
	{2048	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{4096	,0	,0	,NULL	,NULL	,NULL	,NULL},			//4KB
	{8192	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{16384	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{32768	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{65536	,0	,0	,NULL	,NULL	,NULL	,NULL},			//64KB
	{131072	,0	,0	,NULL	,NULL	,NULL	,NULL},			//128KB
	{262144	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{524288	,0	,0	,NULL	,NULL	,NULL	,NULL},
	{1048576,0	,0	,NULL	,NULL	,NULL	,NULL},			//1MB
};
	memcpy(t, kmalloc_cache_size, sizeof(t));
	struct Page * page = NULL;
	unsigned long * virtual = NULL; // get a free page and set to empty page table and return the virtual address
	unsigned long i,j;

	unsigned long tmp_address = memory_management_struct.end_of_struct;

	for(i = 0;i < 16;i++)
	{	// 放在内存管理结构体后面，然后将结构体往后延伸
		kmalloc_cache_size[i].cache_pool = (struct Slab *)memory_management_struct.end_of_struct;
		memory_management_struct.end_of_struct = memory_management_struct.end_of_struct + sizeof(struct Slab) + sizeof(long) * 10;

		list_init(&kmalloc_cache_size[i].cache_pool->list);	

		kmalloc_cache_size[i].cache_pool->using_count = 0;
		kmalloc_cache_size[i].cache_pool->free_count  = PAGE_2M_SIZE / kmalloc_cache_size[i].size;
		// 位图大小取决于当前页可容纳的 slab_cache->size 数量，按照 8 字节对齐
		kmalloc_cache_size[i].cache_pool->color_length =((PAGE_2M_SIZE / 
			kmalloc_cache_size[i].size + sizeof(unsigned long) * 8 - 1) >> 6) << 3;

		kmalloc_cache_size[i].cache_pool->color_count = kmalloc_cache_size[i].cache_pool->free_count;
		kmalloc_cache_size[i].cache_pool->color_map = (unsigned long *)memory_management_struct.end_of_struct;

		memory_management_struct.end_of_struct = (unsigned long)(memory_management_struct.end_of_struct + 
			kmalloc_cache_size[i].cache_pool->color_length + sizeof(long) * 10) & ( ~ (sizeof(long) - 1));
		// 置 1
		memset(kmalloc_cache_size[i].cache_pool->color_map,0xff,
			kmalloc_cache_size[i].cache_pool->color_length);
		// 清零
		for(j = 0;j < kmalloc_cache_size[i].cache_pool->color_count;j++)
			*(kmalloc_cache_size[i].cache_pool->color_map + (j >> 6)) ^= 1UL << j % 64;

		kmalloc_cache_size[i].total_free = kmalloc_cache_size[i].cache_pool->color_count;
		kmalloc_cache_size[i].total_using = 0;
	}

	////////////	init page for kernel code and memory management struct

	i = Virt_To_Phy(memory_management_struct.end_of_struct) >> PAGE_2M_SHIFT;
	// 下边界对齐，初始化 slab 池对应空间在全局内存管理的位图情况
	for(j = PAGE_2M_ALIGN(Virt_To_Phy(tmp_address)) >> PAGE_2M_SHIFT;j <= i;j++)
	{
		page =  memory_management_struct.pages_struct + j;
		*(memory_management_struct.bits_map + ((page->PHY_address >> PAGE_2M_SHIFT) >> 6)) |= 1UL << (page->PHY_address >> PAGE_2M_SHIFT) % 64;
		page->zone_struct->page_using_count++;
		page->zone_struct->page_free_count--;
		page_init(page,PG_PTable_Maped | PG_Kernel_Init | PG_Kernel);
	}

	printf_color(BLACK, ORANGE, "2.memory_management_struct.bits_map:%018X\t\
		zone_struct->page_using_count:%d\tzone_struct->page_free_count:%d\n",
		*memory_management_struct.bits_map,memory_management_struct.zones_struct->page_using_count,
		memory_management_struct.zones_struct->page_free_count);
	// 分配 16 个页给 slab_cache
	for(i = 0;i < 16;i++)
	{	// 从内核后一段地址开始（2MB 对齐）；转化成对应页表号
		virtual = (unsigned long *)((memory_management_struct.end_of_struct + PAGE_2M_SIZE * i + PAGE_2M_SIZE - 1) & PAGE_2M_MASK);
		page = Virt_To_2M_Page(virtual);
		// 将后面的页分配给 slab_cache
		*(memory_management_struct.bits_map + ((page->PHY_address >> PAGE_2M_SHIFT) >> 6)) |= 1UL << (page->PHY_address >> PAGE_2M_SHIFT) % 64;
		page->zone_struct->page_using_count++;
		page->zone_struct->page_free_count--;

		page_init(page,PG_PTable_Maped | PG_Kernel_Init | PG_Kernel);

		kmalloc_cache_size[i].cache_pool->page = page;
		kmalloc_cache_size[i].cache_pool->Vaddress = virtual;
	}

	printf_color(BLACK, ORANGE, "3.memory_management_struct.bits_map:%018X\tzone_struct->page_using_count:%d\t\
		zone_struct->page_free_count:%d\n",*memory_management_struct.bits_map,
		memory_management_struct.zones_struct->page_using_count,
		memory_management_struct.zones_struct->page_free_count);

	printf_color(BLACK, ORANGE, "start_code:%018X,end_code:%018X,end_data:%018X,end_brk:%018X,\
		end_of_struct:%018X\n",memory_management_struct.start_code,
		memory_management_struct.end_code,memory_management_struct.end_data,
		memory_management_struct.end_brk, memory_management_struct.end_of_struct);

	return 1;
}
// slab中内存块 回收
unsigned long kfree(void * address)
{
	int i;
	int index;
	struct Slab * slab = NULL;
	void * page_base_address = (void *)((unsigned long)address & PAGE_2M_MASK);

	for(i = 0;i < 16;i++)
	{
		slab = kmalloc_cache_size[i].cache_pool;
		do
		{	// 查找地址对应的 slab
			if(slab->Vaddress == page_base_address)
			{	// 获取对应位图坐标并清空
				index = (address - slab->Vaddress) / kmalloc_cache_size[i].size;
				*(slab->color_map + (index >> 6)) ^= 1UL << index % 64;

				slab->free_count++;
				slab->using_count--;
				kmalloc_cache_size[i].total_free++;
				kmalloc_cache_size[i].total_using--;
				// 释放后如果为空；slab 池中总量大于单个的 1.5 倍；不是第一个 slab；那么释放这个 slab
				if((slab->using_count == 0) && (kmalloc_cache_size[i].total_free >= 
					slab->color_count * 3 / 2) && (kmalloc_cache_size[i].cache_pool != slab))
				{
					switch(kmalloc_cache_size[i].size)
					{
						////////////////////slab + map in 2M page
						case 32:
						case 64:
						case 128:
						case 256:	
						case 512:
							list_del(&slab->list);
							kmalloc_cache_size[i].total_free -= slab->color_count;

							page_clean(slab->page);
							free_pages(slab->page,1);
							break;
				
						default:
							list_del(&slab->list);
							kmalloc_cache_size[i].total_free -= slab->color_count;

							kfree(slab->color_map);

							page_clean(slab->page);
							free_pages(slab->page,1);
							kfree(slab);
							break;
					}
 
				}

				return 1;
			}
			else
				slab = container_of(list_next(&slab->list),struct Slab,list);				

		}while(slab != kmalloc_cache_size[i].cache_pool);
	
	}
	
	printf_color(BLACK, RED, "kfree() ERROR: can`t free memory\n");
	
	return 0;
}
// 申请新的 slab
struct Slab * kmalloc_create(unsigned long size)
{
	int i;
	struct Slab * slab = NULL;
	struct Page * page = NULL;
	unsigned long * vaddresss = NULL;
	long structsize = 0;
	// 在 ZONE_NORMAL 中申请一个 FLAG 为 0 的页面
	page = alloc_pages(ZONE_NORMAL,1, 0);
	
	if(page == NULL)
	{
		printf_color(BLACK, RED, "kmalloc_create()->alloc_pages()=>page == NULL\n");
		return NULL;
	}
	
	page_init(page,PG_Kernel);

	switch(size)
	{
		////////////////////slab + map in 2M page
		// 32B ~ 512B 区间执行相同的
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
			// 获取虚拟地址
			vaddresss = Phy_To_Virt(page->PHY_address);
			// Slab + 位图大小（单位：字节）
			structsize = sizeof(struct Slab) + PAGE_2M_SIZE / size / 8;
			// 得到 slab 的起始地址（位于页尾部）；位图位于 slab 后面
			slab = (struct Slab *)((unsigned char *)vaddresss + PAGE_2M_SIZE - structsize);
			slab->color_map = (unsigned long *)((unsigned char *)slab + sizeof(struct Slab));
			// 参数初始化
			slab->free_count = (PAGE_2M_SIZE - (PAGE_2M_SIZE / size / 8) - sizeof(struct Slab)) / size;
			slab->using_count = 0;
			slab->color_count = slab->free_count;
			slab->Vaddress = vaddresss;
			slab->page = page;
			list_init(&slab->list);
			// 置位
			slab->color_length = ((slab->color_count + sizeof(unsigned long) * 8 - 1) >> 6) << 3;
			memset(slab->color_map,0xff,slab->color_length);
			// 置位再清空？神经病
			for(i = 0;i < slab->color_count;i++)
				*(slab->color_map + (i >> 6)) ^= 1UL << i % 64;

			break;

		///////////////////kmalloc slab and map,not in 2M page anymore

		case 1024:		//1KB
		case 2048:
		case 4096:		//4KB
		case 8192:
		case 16384:

		//////////////////color_map is a very short buffer.

		case 32768:
		case 65536:
		case 131072:		//128KB
		case 262144:
		case 524288:
		case 1048576:		//1MB
			// 当内存单元在 1KB 以上时，使用 kmalloc 为 slab 和位图申请内存，减少内存浪费
			slab = (struct Slab *)kmalloc(sizeof(struct Slab),0);
			slab->free_count = PAGE_2M_SIZE / size;
			slab->using_count = 0;
			slab->color_count = slab->free_count;
			slab->color_length = ((slab->color_count + sizeof(unsigned long) * 8 - 1) >> 6) << 3;
			// 用 kmalloc 为位图申请内存
			slab->color_map = (unsigned long *)kmalloc(slab->color_length,0);
			memset(slab->color_map,0xff,slab->color_length);

			slab->Vaddress = Phy_To_Virt(page->PHY_address);
			slab->page = page;
			list_init(&slab->list);

			for(i = 0;i < slab->color_count;i++)
				*(slab->color_map + (i >> 6)) ^= 1UL << i % 64;

			break;

		default:

			printf_color(BLACK, RED, "kmalloc_create() ERROR: wrong size:%08d\n",size);
			free_pages(page,1);
			
			return NULL;
	}	
	
	return slab;
}

void free_pages(struct Page * page,int number)
{	
	int i = 0;
	
	if(page == NULL)
	{
		printf_color(BLACK, RED, "free_pages() ERROR: page is invalid\n");
		return ;
	}	

	if(number >= 64 || number <= 0)
	{
		printf_color(BLACK, RED, "free_pages() ERROR: number is invalid\n");
		return ;	
	}
	
	for(i = 0;i<number;i++,page++)
	{
		*(memory_management_struct.bits_map + ((page->PHY_address >> PAGE_2M_SHIFT) >> 6)) &= ~(1UL << (page->PHY_address >> PAGE_2M_SHIFT) % 64);
		page->zone_struct->page_using_count--;
		page->zone_struct->page_free_count++;
		page->attribute = 0;
	}
}
// 在 slab 池中的 slab 中申请新的内存，大小为 size，返回虚拟地址
void * kmalloc(unsigned long size,unsigned long gfp_flages)
{
	int i,j;
	struct Slab * slab = NULL;
	// 先判断范围，slab_cache 的最大值不超过 1MB
	if(size > 1048576)
	{
		printf_color(BLACK, RED, "kmalloc() ERROR: kmalloc size too long:%08d\n",size);
		return NULL;
	}
	// 寻找第一个符合大小的 slab_cache
	for(i = 0;i < 16;i++)
		if(kmalloc_cache_size[i].size >= size)
			break;
	slab = kmalloc_cache_size[i].cache_pool;
	// 如果当前 slab_cache 没用光
	if(kmalloc_cache_size[i].total_free != 0)
	{
		do
		{	// 如果当前 slab 页内没有空余位置，寻找下一个 slab
			if(slab->free_count == 0)
				slab = container_of(list_next(&slab->list),struct Slab,list);
			else
				break;
		}while(slab != kmalloc_cache_size[i].cache_pool);	
	}
	else
	{	// 如果 slab_cache 用尽，申请新的 slab
		slab = kmalloc_create(kmalloc_cache_size[i].size);
		
		if(slab == NULL)
		{
			printf_color(BLACK, BLUE, "kmalloc()->kmalloc_create()=>slab == NULL\n");
			return NULL;
		}
		
		kmalloc_cache_size[i].total_free += slab->color_count;
		printf_color(BLACK, BLUE, "kmalloc()->kmalloc_create()<=size:%#010x\n",kmalloc_cache_size[i].size);///////
		
		list_add_to_before(&kmalloc_cache_size[i].cache_pool->list,&slab->list);
	}
	// 在位图中寻找空闲位置
	for(j = 0;j < slab->color_count;j++)
	{
		if(*(slab->color_map + (j >> 6)) == 0xffffffffffffffffUL)
		{
			j += 63;
			continue;
		}
		
		if( (*(slab->color_map + (j >> 6)) & (1UL << (j % 64))) == 0 )
		{
			*(slab->color_map + (j >> 6)) |= 1UL << (j % 64);
			slab->using_count++;
			slab->free_count--;

			kmalloc_cache_size[i].total_free--;
			kmalloc_cache_size[i].total_using++;

			return (void *)((char *)slab->Vaddress + kmalloc_cache_size[i].size * j);
		}
	}

	printf_color(BLACK, BLUE, "kmalloc() ERROR: no memory can alloc\n");
	return NULL;
}

unsigned long page_clean(struct Page * page)
{
	page->reference_count--;
	page->zone_struct->total_pages_link--;

	if(!page->reference_count)
	{
		page->attribute &= PG_PTable_Maped;
	}
	
	return 1;
}

unsigned long get_page_attribute(struct Page * page)
{
	if(page == NULL)
	{
		color_printk(RED,BLACK,"get_page_attribute() ERROR: page == NULL\n");
		return 0;
	}
	else
		return page->attribute;
}

unsigned long set_page_attribute(struct Page * page,unsigned long flags)
{
	if(page == NULL)
	{
		color_printk(RED,BLACK,"set_page_attribute() ERROR: page == NULL\n");
		return 0;
	}
	else
	{
		page->attribute = flags;
		return 1;
	}
}