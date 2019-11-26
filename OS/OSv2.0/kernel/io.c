#include "io.h"
#include "font.h"
#include "graph.h"
#include <stdarg.h>

void putchar(char c)
{
    putchar_color(c, BLACK, WHITE);
}

void putchar_color(char c, uint32 BackColor, uint32 ForeColor)
{
    int i, j;
    for (i = 0;i < 16;i++)
    {
        int tmp = 0x100;
        int y = Pos.XResolution * (Pos.YPosition + i);
        for (j = 0;j < 8; j++)
        {
            tmp = tmp >> 1;
            if (font_ascii[c][i] & tmp)
                Pos.FB_addr[y + Pos.XPosition + j] = (uint32)ForeColor;
            else
                Pos.FB_addr[y + Pos.XPosition + j] = (uint32)BackColor;
        }
    }
    TranslateX(Pos.XCharSize);
}

void puts_color(char *str, uint32 BackColor, uint32 ForeColor)
{
    while(*str)
    {
        putchar_color(*str, BackColor, ForeColor);
        str++;
    }
}

void puts(char *str)
{
    puts_color(str, BLACK, WHITE);
}

void printf_color(uint32 BackColor, uint32 ForeColor, char *str, ...)
{
    va_list arg;
    va_start(arg, str);

    while(*str)
    {
        if (*str == '%')
        {
            str++;
            if (*str == 'd')
                puti_color(va_arg(arg, int), BackColor, ForeColor);
            else if (*str == 'c')
                putchar_color((char)va_arg(arg, int), BackColor, ForeColor);
            else if (*str == 's')
                puts_color((char *)va_arg(arg, char*), BackColor, ForeColor);
            else if (*str == 'x')
                putx_color(va_arg(arg, int), BackColor, ForeColor, 0);
            else if (*str == 'X')
                putx_color(va_arg(arg, int), BackColor, ForeColor, 1);
        }
        else if (*str == '\n')
        {
            Pos.YPosition += Pos.YCharSize;
            Pos.XPosition = 0;
            Pos.YPosition %= Pos.YResolution;
        }
        else if (*str == '\t')
        {
            char tmp[50];
            int i, cnt = Pos.XCharSize - Pos.XPosition % (Pos.XCharSize * 4);
            for (i = 0;i < cnt; i++)
                tmp[i] = ' ';
            tmp[i] = '\0';
            puts_color(tmp, BackColor, ForeColor);
        }
        else if (*str == '\b')
            TranslateX(-1);
        else
            putchar_color(*str, BackColor, ForeColor);
        str++;
    }
    va_end(arg);
}

void printf(char *str, ...)
{
    va_list arg;
    va_start(arg, str);

    while(*str)
    {
        if (*str == '%')
        {
            str++;
            if (*str == 'd')
                puti(va_arg(arg, int));
            else if (*str == 'c')
                putchar((char)va_arg(arg, int));
            else if (*str == 's')
                puts((char *)va_arg(arg, char*));
            else if (*str == 'x')
                putx(va_arg(arg, int), 0);
            else if (*str == 'X')
                putx(va_arg(arg, int), 1);
        }
        else if (*str == '\n')
        {
            Pos.YPosition += Pos.YCharSize;
            Pos.XPosition = 0;
            Pos.YPosition %= Pos.YResolution;
        }
        else if (*str == '\t')
        {
            char tmp[50];
            int i, cnt = Pos.XCharSize - Pos.XPosition % (Pos.XCharSize * 4);
            for (i = 0;i < cnt; i++)
                tmp[i] = ' ';
            tmp[i] = '\0';
            puts(tmp);
        }
        else if (*str == '\b')
            TranslateX(-1);
        else
            putchar(*str);
        str++;
    }
    va_end(arg);
}

void puti_color(int n, uint32 BackColor, uint32 ForeColor)
{
    int cnt = 0, i;
    char num[25];
    int tmp_n = n;
    n = n < 0 ? -n : n;

    while (n / 10)
    {
        num[cnt++] = '0' + n % 10;
        n /= 10;
    }
    num[cnt++] = '0' + n % 10;
    if (tmp_n < 0)
        num[cnt++] = '-';
    int t = cnt - 1;
    for (i = 0;i < t;i++)
    {
        t = cnt - i - 1;
        char tmp = num[i];
        num[i] = num[t];
        num[t] = tmp;
    }
    num[cnt] = '\0';
    puts_color(num, BackColor, ForeColor);
}

void puti(int n)
{
    puti_color(n, BLACK, WHITE);
}

void putx_color(int n, uint32 BackColor, uint32 ForeColor, int isuper)
{
    int cnt = 0, i;
    char num[25];
    int tmp_n = n;
    n = n < 0 ? -n : n;

    while (n / 16)
    {
        if (n % 16 < 10)
            num[cnt++] = '0' + n % 16;
        else
        {
            if (isuper)
                num[cnt++] = 'A' + n % 16 - 10;
            else
                num[cnt++] = 'a' + n % 16 - 10;
        }
        n /= 16;
    }
    if (n % 16 < 10)
        num[cnt++] = '0' + n % 16;
    else
    {
        if (isuper)
            num[cnt++] = 'A' + n % 16 - 10;
        else
            num[cnt++] = 'a' + n % 16 - 10;
    }
    if (tmp_n < 0)
        num[cnt++] = '-';
    int t = cnt - 1;
    for (i = 0;i < t;i++)
    {
        t = cnt - i - 1;
        char tmp = num[i];
        num[i] = num[t];
        num[t] = tmp;
    }
    num[cnt] = '\0';
    puts_color(num, BackColor, ForeColor);
}

void putx(int n, int isuper)
{
    putx_color(n, BLACK, WHITE, isuper);
}
