#include "graph.h"
#include "abnormal.h"
#include "memory.h"
#include "interrupt.h"
#include "keyboard.h"
#include "mouse.h"
#include "disk.h"
#include "timer.h"

struct Global_Memory_Descriptor memory_management_struct = {{0}, 0};

void Start_Kernel(void)
{
	struct Page * page = NULL;
	void * tmp = NULL;
	struct Slab *slab = NULL;
	int i;

    InitGraph((uint32 *)0xffff800003000000);	// 显存初始化
    sys_vector_init();							// 系统调用初始化
    init_cpu();									// cpu 信息获取
	load_TR(10);								// 加载 TSS；设置 TSS
    set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00,
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00);
    
    init_memory();								// 内存管理初始化
    
    char msg[] = "em....";
    printf_color(BLACK, RED, "HEHEHE\n\t%s%d", msg, -123);

    init_interrupt();

    // slab 内存池初始化
 //    printf_color(BLACK, RED, "slab init \n");
	slab_init();
    keyboard_init();
    InitTimer();
    // mouse_init();
    // disk_init();
	
	// printf_color(BLACK, WHITE, "kmalloc test\n");
	// for(i = 0;i< 16;i++)
	// {
	// 	printf_color(BLACK, RED, "size:%010X\t",kmalloc_cache_size[i].size);
	// 	printf_color(BLACK, RED, "color_map(before):%018x\t",*kmalloc_cache_size[i].cache_pool->color_map);
	// 	tmp = kmalloc(kmalloc_cache_size[i].size,0);
	// 	if(tmp == NULL)
	// 		printf_color(BLACK, RED, "kmalloc size:%010X ERROR\n",kmalloc_cache_size[i].size);
	// 	printf_color(BLACK, RED, "color_map(middle):%018x\t",*kmalloc_cache_size[i].cache_pool->color_map);
	// 	kfree(tmp);
	// 	printf_color(BLACK, RED, "color_map(after):%018x\n",*kmalloc_cache_size[i].cache_pool->color_map);
	// }
	// page = alloc_pages(ZONE_NORMAL,63, 0);
	// page = alloc_pages(ZONE_NORMAL,63, 0);

	// printf_color(BLACK, ORANGE, "4.memory_management_struct.bits_map:%018X\t\
	// 	memory_management_struct.bits_map+1:%018X\tmemory_management_struct.bits_map+2:\
	// 	%018X\tzone_struct->page_using_count:%d\tzone_struct->page_free_count:%d\n",
	// 	*memory_management_struct.bits_map,*(memory_management_struct.bits_map+1),
	// 	*(memory_management_struct.bits_map+2),memory_management_struct.zones_struct->
	// 	page_using_count,memory_management_struct.zones_struct->page_free_count);

	// for(i = 80;i <= 85;i++)
	// {
	// 	printf_color(BLACK, INDIGO, "page%03d attribute:%018X address:%018X\t",
	// 		i,(memory_management_struct.pages_struct + i)->
	// 		attribute,(memory_management_struct.pages_struct + i)->PHY_address);
	// 	i++;
	// 	printf_color(BLACK, INDIGO, "page%03d attribute:%018X address:%018X\n",i,
	// 		(memory_management_struct.pages_struct + i)->attribute,
	// 		(memory_management_struct.pages_struct + i)->PHY_address);
	// }

	// for(i = 140;i <= 145;i++)
	// {
	// 	printf_color(BLACK, INDIGO, "page%03d attribute:%018X address:%018X\t",i,
	// 		(memory_management_struct.pages_struct + i)->attribute,
	// 		(memory_management_struct.pages_struct + i)->PHY_address);
	// 	i++;
	// 	printf_color(BLACK, INDIGO, "page%03d attribute:%018X address:%018X\n",i,
	// 		(memory_management_struct.pages_struct + i)->attribute,
	// 		(memory_management_struct.pages_struct + i)->PHY_address);
	// }

	// free_pages(page,1);
	
	// printf_color(BLACK, ORANGE, "5.memory_management_struct.bits_map:%018X\t\
	// 	memory_management_struct.bits_map+1:%018X\tmemory_management_struct.bits_map+2:\
	// 	%018X\tzone_struct->page_using_count:%d\tzone_struct->page_free_count:%d\n",
	// 	*memory_management_struct.bits_map,*(memory_management_struct.bits_map+1),
	// 	*(memory_management_struct.bits_map+2),memory_management_struct.zones_struct->
	// 	page_using_count,memory_management_struct.zones_struct->page_free_count);

	// for(i = 75;i <= 85;i++)
	// {
	// 	printf_color(BLACK, INDIGO, "page%03d attribute:%018X address:%018X\t",i,
	// 		(memory_management_struct.pages_struct + i)->attribute,
	// 		(memory_management_struct.pages_struct + i)->PHY_address);
	// 	i++;
	// 	printf_color(BLACK, INDIGO, "page%03d attribute:%018X address:%018X\n",i,
	// 		(memory_management_struct.pages_struct + i)->attribute,
	// 		(memory_management_struct.pages_struct + i)->PHY_address);
	// }
	
	// page = alloc_pages(ZONE_UNMAPED,63, 0);

	// printf_color(BLACK, ORANGE, "6.memory_management_struct.bits_map:%018X\t\
	// 	memory_management_struct.bits_map+1:%018X\tzone_struct->page_using_count:%d\t\
	// 	zone_struct->page_free_count:%d\n",*(memory_management_struct.bits_map + 
	// 		(page->PHY_address >> PAGE_2M_SHIFT >> 6)) , *(memory_management_struct.bits_map
	// 		 + 1 + (page->PHY_address >> PAGE_2M_SHIFT >> 6)) ,
	// 		(memory_management_struct.zones_struct + ZONE_UNMAPED_INDEX)->page_using_count,
	// 		(memory_management_struct.zones_struct + ZONE_UNMAPED_INDEX)->page_free_count );

	// free_pages(page,1);

	// printf_color(BLACK, ORANGE, "7.memory_management_struct.bits_map:%018X\t\
	// 	memory_management_struct.bits_map+1:%018X\tzone_struct->page_using_count:%d\t\
	// 	zone_struct->page_free_count:%d\n",*(memory_management_struct.bits_map + 
	// 		(page->PHY_address >> PAGE_2M_SHIFT >> 6)) , *(memory_management_struct.bits_map
	// 		 + 1 + (page->PHY_address >> PAGE_2M_SHIFT >> 6)) ,
	// 		(memory_management_struct.zones_struct + ZONE_UNMAPED_INDEX)->page_using_count,
	// 		(memory_management_struct.zones_struct + ZONE_UNMAPED_INDEX)->page_free_count );

    // init_process();
	
    while(1){
        decode_keyboard();
        // decode_mouse();
    }
}
