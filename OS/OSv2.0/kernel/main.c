#include "graph.h"
#include "interrupt.h"
#include "memory.h"

struct Global_Memory_Descriptor memory_management_struct = {{0}, 0};

void Start_Kernel(void)
{
    InitGraph((uint32 *)0xffff800000a00000);
    sys_vector_init();
    set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00,
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00);

    init_memory();

    char msg[] = "world";
    printf_color(BLACK, RED, "Fuck\n\t%s%d", msg, -123);
	while(1)
		;
}
