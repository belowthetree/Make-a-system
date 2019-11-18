#include "main.h"
#include "desc.h"
#include "keyboard.h"
#include "tool.h"
#include "process.h"
#include "init.h"
#include "memory_manager.h"

int timer;

void main()
{
    font_x = font_y = 0;
    InitScreen();
    InitPIC();
    InitGDT();
    InitIDT();
    unsigned int i = getmem(0x00400000, 0xbfffffff);
    MAX_PHY_SIZE = i / (1024 * 1024);
    InitMemory();

    //InitTimer(1);
    InitProcess();
    timer = 0;
    io_sti();
    

    for(;;)
        io_hlt();
}

void Clock()
{
    io_cli();
    char msg[] = "Error\n";
    printf(msg);
    io_sti();
    return;
}

void Clock2(pt_regs * regs)
{
    char msg[] = "Tick: ";
    printf(msg);
    printi(timer++, 1);
    return;
}