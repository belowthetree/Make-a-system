#include "graph.h"

void Start_Kernel(void)
{
    InitGraph((uint32 *)0xffff800000a00000);
    char msg[] = "world";
    printf_color(BLACK, RED, "Fuck\n\t%s%d", msg, -123);
	while(1)
		;
}
