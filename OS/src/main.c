#include "main.h"
#include "desc.h"
#include "keyboard.h"
#include "tool.h"
#include "process.h"

void main()
{
    font_x = font_y = 0;
    InitScreen();
    InitPIC();
    InitGDT();
    InitIDT();
    io_sti();

    unsigned int i = getmem(0x00400000, 0xbfffffff);
    printi(i / (1024 * 1024), 3);

    InitProcess();

    for(;;)
        io_hlt();
}

void Clock()
{
    printi(12, 3);
    io_out8(0x20, 0x61);
    io_sti();
    return;
}