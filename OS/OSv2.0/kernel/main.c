#include "graph.h"
#include "abnormal.h"
#include "memory.h"
#include "interrupt.h"

struct Global_Memory_Descriptor memory_management_struct = {{0}, 0};

void Start_Kernel(void)
{
    InitGraph((uint32 *)0xffff800000a00000);
    sys_vector_init();
	load_TR(8);
    set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00,
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00);

    init_memory();

    char msg[] = "world";
    printf_color(BLACK, RED, "Fuck\n\t%s%d", msg, -123);

    init_interrupt();
    init_process();

 //    printf_color(BLACK, RED, "memory_management_struct.bits_map:%018X\n",*memory_management_struct.bits_map);
	// printf_color(BLACK, RED, "memory_management_struct.bits_map:%018X\n",*(memory_management_struct.bits_map + 1));

	// struct Page * page = NULL;
	// page = alloc_pages(ZONE_NORMAL,64,PG_PTable_Maped | PG_Active | PG_Kernel);
	// int i;
	// for(i = 0;i <= 64;i++)
	// {
	// 	printf_color(BLACK, INDIGO, "page%d\tattribute:%018X\taddress:%018X\t",i,(page + i)->attribute,(page + i)->PHY_address);
	// 	i++;
	// 	printf_color(BLACK, INDIGO, "page%d\tattribute:%018X\taddress:%018X\n",i,(page + i)->attribute,(page + i)->PHY_address);
	// }

	// printf_color(BLACK, RED, "memory_management_struct.bits_map:%018X\n",*memory_management_struct.bits_map);
	// printf_color(BLACK, RED, "memory_management_struct.bits_map:%018X\n",*(memory_management_struct.bits_map + 1));


	while(1)
		;
}
