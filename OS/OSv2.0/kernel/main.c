#include "graph.h"
#include "interrupt.h"

void Start_Kernel(void)
{
    InitGraph((uint32 *)0xffff800000a00000);
    sys_vector_init();
    set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00,
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
     0xffff800000007c00);
	int i = *(int *)0xfff80000aa00000;
    char msg[] = "world";
    printf_color(BLACK, RED, "Fuck\n\t%s%d", msg, -123);
	while(1)
		;
}
