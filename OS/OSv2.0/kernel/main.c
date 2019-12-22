#include "graph.h"
#include "abnormal.h"
#include "memory.h"
#include "interrupt.h"

struct Global_Memory_Descriptor memory_management_struct = {{0}, 0};

void Start_Kernel(void)
{
	struct Page * page = NULL;
	void * tmp = NULL;
	struct Slab *slab = NULL;

    InitGraph((uint32 *)0xffff800003000000);	// 显存初始化
    sys_vector_init();							// 系统调用初始化
    init_cpu();									// cpu 信息获取
	load_TR(10);								// 加载 TSS；设置 TSS
    set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00,
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00);

    init_memory();								// 内存管理初始化
    
    char msg[] = "world";
    printf_color(BLACK, RED, "Fuck\n\t%s%d", msg, -123);

    init_interrupt();
    // slab 内存池初始化
    slab_init();

    printf_color(BLACK, ORANGE, "4.memory_management_struct.bits_map:%018uX\tmemory_management_struct.bits_map+1:%018uX\tmemory_management_struct.bits_map+2:%018uX\tzone_struct->page_using_count:%d\tzone_struct->page_free_count:%d\n",*memory_management_struct.bits_map,*(memory_management_struct.bits_map+1),*(memory_management_struct.bits_map+2),memory_management_struct.zones_struct->page_using_count,memory_management_struct.zones_struct->page_free_count);

	printf_color(BLACK, WHITE, "kmalloc test\n");
	int i;
	for(i = 0;i< 16;i++)
	{
		printf_color(BLACK, RED, "size:%010uX\t",kmalloc_cache_size[i].size);
		printf_color(BLACK, RED, "color_map(before):%018uX\t",*kmalloc_cache_size[i].cache_pool->color_map);
		tmp = kmalloc(kmalloc_cache_size[i].size,0);
		if(tmp == NULL)
			printf_color(BLACK, RED, "kmalloc size:%010uX ERROR\n",kmalloc_cache_size[i].size);
		printf_color(BLACK, RED, "color_map(middle):%018uX\t",*kmalloc_cache_size[i].cache_pool->color_map);
		kfree(tmp);
		printf_color(BLACK, RED, "color_map(after):%018uX\n",*kmalloc_cache_size[i].cache_pool->color_map);
	}

	kmalloc(kmalloc_cache_size[15].size,0);
	kmalloc(kmalloc_cache_size[15].size,0);
	kmalloc(kmalloc_cache_size[15].size,0);
	kmalloc(kmalloc_cache_size[15].size,0);
	kmalloc(kmalloc_cache_size[15].size,0);
	kmalloc(kmalloc_cache_size[15].size,0);
	kmalloc(kmalloc_cache_size[15].size,0);


	printf_color(BLACK, RED, "color_map(0):%018uX,%018uX\n",kmalloc_cache_size[15].cache_pool->color_map,*kmalloc_cache_size[15].cache_pool->color_map);
	slab = container_of(list_next(&kmalloc_cache_size[15].cache_pool->list),struct Slab,list);
	printf_color(BLACK, RED, "color_map(1):%018uX,%018uX\n",slab->color_map,*slab->color_map);
	slab = container_of(list_next(&slab->list),struct Slab,list);
	printf_color(BLACK, RED, "color_map(2):%018uX,%018uX\n",slab->color_map,*slab->color_map);
	slab = container_of(list_next(&slab->list),struct Slab,list);
	printf_color(BLACK, RED, "color_map(3):%018uX,%018uX\n",slab->color_map,*slab->color_map);
	

    init_process();

 //    printf_color(BLACK, RED, "memory_management_struct.bits_map:%018uX\n",*memory_management_struct.bits_map);
	// printf_color(BLACK, RED, "memory_management_struct.bits_map:%018uX\n",*(memory_management_struct.bits_map + 1));

	// struct Page * page = NULL;
	// page = alloc_pages(ZONE_NORMAL,64,PG_PTable_Maped | PG_Active | PG_Kernel);
	// int i;
	// for(i = 0;i <= 64;i++)
	// {
	// 	printf_color(BLACK, INDIGO, "page%d\tattribute:%018uX\taddress:%018uX\t",i,(page + i)->attribute,(page + i)->PHY_address);
	// 	i++;
	// 	printf_color(BLACK, INDIGO, "page%d\tattribute:%018uX\taddress:%018uX\n",i,(page + i)->attribute,(page + i)->PHY_address);
	// }

	// printf_color(BLACK, RED, "memory_management_struct.bits_map:%018uX\n",*memory_management_struct.bits_map);
	// printf_color(BLACK, RED, "memory_management_struct.bits_map:%018uX\n",*(memory_management_struct.bits_map + 1));


	while(1)
		;
}
