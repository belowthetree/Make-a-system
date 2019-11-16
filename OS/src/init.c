#include "main.h"
#include "desc.h"


void WriteMem8(char *addr, char value)
{
    *addr = value;
    return;
}

void InitTimer(uint32 frequency)
{
    // printi(Clock2, 1);
    // register_interrupt_handler(IRQ0, Clock2);
    interrupt_handlers[IRQ0] = Clock2;

    uint32 divisor = 1193180000 / frequency;
    io_out8(0x43, 0x36);

    uint8 low = (uint8) (divisor & 0xff);
    uint8 high = (uint8) ((divisor >> 8) & 0xff);

    io_out8(0x40, low);
    io_out8(0x40, high);
    return ;
}