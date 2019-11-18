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
    printi(i / (1024 * 1024), 3);
    MAX_PHY_SIZE = i / (1024 * 1024);

    //InitTimer(1);
    //InitProcess();
    timer = 0;
    io_sti();

    printf("Malloc for first page, addr: ");
    printi(MallocPage(), 1);
    printf("Malloc for second page, addr: ");
    printi(MallocPage(), 1);

    for(;;)
        io_hlt();
}

void Clock()
{
    io_cli();
    static int x = 0;
    printi(&x, 0);
    font_y += font_x / 80;
    font_x = font_x % 80;
    io_sti();
    return;
}

void Clock2(pt_regs * regs)
{
    char msg[] = "Tick: ";
    printf("%d", timer);
    printi(timer++, 1);
    return;
}