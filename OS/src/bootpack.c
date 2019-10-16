#include "bootpack.h"
#include <stdio.h>

extern struct FIFO8 keyfifo, mousefifo;
void enable_mouse();
void init_keyboard();

void HariMain()
{
    struct BOOTINFO *binfo = (struct BOOTINFO*)ADR_BOOTINFO;
    
}
