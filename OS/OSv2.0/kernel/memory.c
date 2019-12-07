#include "memory.h"
#include "type.h"
#include "graph.h"
#include "io.h"

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
	unsigned long TotalMem = 0 ;
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

		memory_management_struct.e820[i].address += p->address;
		memory_management_struct.e820[i].length	 += p->length;
		memory_management_struct.e820[i].type	 = p->type;
		memory_management_struct.e820_length 	 = i;

		p++;
		if(p->type > 4 || p->length == 0 || p->type < 1)
			break;
	}

	printf_color(BLACK, ORANGE, "OS Can Used Total RAM:%X\n",TotalMem);

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
	for(i = 0;i < 10;i++)
		*(Phy_To_Virt(Global_CR3)  + i) = 0UL;
	
	flush_tlb();
}

unsigned long page_init(struct Page * page,unsigned long flags)
{
	if(!page->attribute)
	{
		*(memory_management_struct.bits_map + ((page->PHY_address >> PAGE_2M_SHIFT) >> 6)) |= 1UL << (page->PHY_address >> PAGE_2M_SHIFT) % 64;
		page->attribute = flags;
		page->reference_count++;
		page->zone_struct->page_using_count++;
		page->zone_struct->page_free_count--;
		page->zone_struct->total_pages_link++;
	}
	else if((page->attribute & PG_Referenced) || (page->attribute & PG_K_Share_To_U) || (flags & PG_Referenced) || (flags & PG_K_Share_To_U))
	{
		page->attribute |= flags;
		page->reference_count++;
		page->zone_struct->total_pages_link++;
	}
	else
	{
		*(memory_management_struct.bits_map + ((page->PHY_address >> PAGE_2M_SHIFT) >> 6)) |= 1UL << (page->PHY_address >> PAGE_2M_SHIFT) % 64;	
		page->attribute |= flags;
	}
	return 0;
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