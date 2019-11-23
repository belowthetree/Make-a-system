#include "main.h"

void SetVRAM(int x, int y, char c, char background, char color)
{
    short value = (background << 4) + color;
    value = (value << 8) + c;
    short* str = (short *)(VRAM + (y * 80 + x)*2);
    *str = value;
    return;
}

void InitScreen()
{
    int x, y;
    for(y = 0;y < 240;y++)
    {
        for(x = 0;x < 320;x++)
        {
            SetVRAM(x, y, ' ', BLACK, BLACK);
        }
    }
    return;
}

void InitPallete()
{
    int i, eflags;
    eflags = io_load_eflags();
    io_cli();

    // 待完成

    io_store_eflags(eflags);
    return;
}