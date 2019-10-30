#include "main.h"
#include "int.h"

void main()
{
    InitScreen();
    InitPIC();
    InitIDT();
    io_sti();
    for(;;)
        io_hlt();
}

void Clock()
{
    SetVRAM(0, 0, 'a', WHITE, DEEP_RED);
    
    return;
}