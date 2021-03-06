#include "tool.h"
#include "main.h"

void printi(int data, int y)
{
    int sum = 0;
    int i;
    for(i = 0;i < 32;i++)
    {
        sum *= 2;
        sum += (data >> (31 - i)) & 1;
    }

    i = 0;
    char str[30];
    while(1)
    {
        str[i++] = sum % 10 + '0';
        if(sum / 10 == 0)
            break;
        sum /= 10;
    }
    
    while(i-->0)
    {
        SetVRAM(font_x++, font_y, str[i], BLACK, WHITE);
    }

    if (y)
    {
        font_y++;
        font_x = 0;
        font_y %= 40;
    }

    return;
}

unsigned int getmem(unsigned int start, unsigned int end)
{
    char flg486 = 0;
    unsigned int eflg, cr0, i;
    eflg = io_load_eflags();
    eflg |= 0x40000;
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if ((eflg & 0x40000) != 0){
        flg486 = 1;
    }
    eflg &= ~0x40000;
    io_store_eflags(eflg);

    if (flg486 != 0)
    {
        cr0 = load_cr0();
        cr0 |= 0x60000000;
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);

    if(flg486 != 0)
    {
        cr0 = load_cr0();
        cr0 &= ~0x60000000;
        store_cr0(cr0);
    }

    return i;
}

unsigned int getmem_sub(unsigned int start, unsigned int end)
{
    unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
    for (i = start;i <= end;i += 0x1000)
    {
        p = (unsigned int *) i;
        old = *p;
        *p = pat0;
        *p ^= 0xffffffff;
        if (*p != pat1)
        {
            not_memory:
            *p = old;
            break;
        }
        *p ^= 0xffffffff;
        if (*p != pat0){
            goto not_memory;
        }
        *p = old;
    }

    return i;
}

void memset(char *str, char value, unsigned int size)
{
    unsigned int i;
    for (i = 0;i < size;++i)
    {
        *(str + i) = value;
    }

    return;
}

void printf(const char *str, ...)
{
    char * esp;
    int i = 4;

    while(*str)
    {
        if (*str == '%')
        {
            ++str;
            if (*str == 'd')
            {
                printi(*(esp + i), 0);
                i += sizeof(int);
            }
            if (*str == 's')
            {
                for (;((char *)(*(int *)esp))[i];i++)
                {
                    SetVRAM(font_x++, font_y, ((char *)(*(int *)esp))[i], BLACK, DEEP_RED);
                    font_y += font_x / 80;
                    font_x %= 80;
                    font_y %= 40;
                }
            }
            else if (*str == 'c')
            {
                SetVRAM(font_x++, font_y, *(esp + i), BLACK, DEEP_RED);
                font_y += font_x / 80;
                font_x %= 80;
                font_y %= 40;
                ++i;
            }
            else
            {
                SetVRAM(font_x++, font_y, *str, BLACK, DEEP_RED);
                font_y += font_x / 80;
                font_x %= 80;
                font_y %= 40;
            }
            ++str;
        }
        else if (*str == '\n')
        {
            font_x = 0;
            ++font_y;
            font_y %= 40;
        }
        else
            SetVRAM(font_x++, font_y, *str, BLACK, DEEP_RED);
        ++str;
    }
}