#include "keyboard.h"
#include "tool.h"
#include "main.h"

void inthandler21(int *esp)
{
    int data;
    io_out8(0x20, 0x61);
    data = io_in8(PORT_KEYDATA);

    printi(data, 2);
    
    return;
}