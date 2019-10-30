#include "tool.h"
#include "main.h"

void prints(char *str)
{
    int x;
    for(x = 0; x < 30;x++)
    {
        if(!str[x])
            break;
        SetVRAM(x, 0, str[x], BLACK, DEEP_RED);
    }

    return;
}

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
    
    int x = 0;
    while(i-->0)
    {
        SetVRAM(x++, y, str[i], BLACK, WHITE);
    }

    return;
}