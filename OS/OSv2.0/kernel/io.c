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
    int zero, cnt;

    while(*str)
    {
        asm volatile("st:");
        if (*str == '%')
        {
            str++;
            zero = cnt = 0;
            if (*str <= '9' && *str >= '0')
            {
                zero = !(*str - '0'), cnt = 0;
                for (;*str <= '9' && *str >= 0;str++)
                {
                    cnt *= 10;
                    cnt += *str - '0';
                }
            }
            if (*str == 'd')
                puti_color(va_arg(arg, int), BackColor, ForeColor, cnt, zero);
            else if (*str == 'c')
                putchar_color((char)va_arg(arg, int), BackColor, ForeColor);
            else if (*str == 's')
                puts_color((char *)va_arg(arg, char*), BackColor, ForeColor);
            else if (*str == 'x')
                putx_color(va_arg(arg, long), BackColor, ForeColor, 0, cnt, zero);
            else if (*str == 'X')
                putx_color(va_arg(arg, long), BackColor, ForeColor, 1, cnt, zero);
            else if (*str == 'l' && *(str + 1) == 'd')
            {
                str++;
                putl_color(va_arg(arg, long), BackColor, ForeColor, cnt, zero);
            }
            else if (*str == 'u')
            {
                str++;
                if (*str == 'd')
                    putui_color(va_arg(arg, int), BackColor, ForeColor, cnt, zero);
                else if (*str == 'x')
                    putux_color(va_arg(arg, long), BackColor, ForeColor, 0, cnt, zero);
                else if (*str == 'X')
                    putux_color(va_arg(arg, long), BackColor, ForeColor, 1, cnt, zero);
                else if (*str == 'l' && *(str + 1) == 'd')
                {
                    str++;
                    putl_color(va_arg(arg, long), BackColor, ForeColor, cnt, zero);
                }
            }
            else
                putchar_color(*str, BackColor, ForeColor);
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
            int i, cnt = 4 - Pos.XPosition / Pos.XCharSize % 4;
            for (i = 0;i < cnt; i++)
                tmp[i] = ' ';
            tmp[i] = '\0';
            puts(tmp);
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
    int zero, cnt;

    while(*str)
    {
        if (*str == '%')
        {
            str++;
            zero = cnt = 0;
            if (*str <= '9' && *str >= '0')
            {
                zero = !(*str - '0'), cnt = 0;
                for (;*str <= '9' && *str >= 0;str++)
                {
                    cnt *= 10;
                    cnt += *str - '0';
                }
            }
            if (*str == 'd')
                puti(va_arg(arg, int), cnt, zero);
            else if (*str == 'c')
                putchar((char)va_arg(arg, int));
            else if (*str == 's')
                puts((char *)va_arg(arg, char*));
            else if (*str == 'x')
                putx(va_arg(arg, long), 0, cnt, zero);
            else if (*str == 'X')
                putx(va_arg(arg, long), 1, cnt, zero);
            else if (*str == 'l' && *(str + 1) == 'd')
            {
                str++;
                putl(va_arg(arg, long), cnt, zero);
            }
            else if (*str == 'u')
            {
                str++;
                if (*str == 'd')
                    putui(va_arg(arg, int), cnt, zero);
                else if (*str == 'x')
                    putux(va_arg(arg, long), 0, cnt, zero);
                else if (*str == 'X')
                    putux(va_arg(arg, long), 1, cnt, zero);
                else if (*str == 'l' && *(str + 1) == 'd')
                {
                    str++;
                    putl(va_arg(arg, long), cnt, zero);
                }
            }
            else
                putchar(*str);
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
            int i, cnt = 4 - Pos.XPosition / Pos.XCharSize % 4;
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

void puti_color(int n, uint32 BackColor, uint32 ForeColor, int align, int zero)
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

    while(align > cnt)
        if (zero)
            num[cnt++] = '0';
        else
            num[cnt++] = ' ';
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

void puti(int n, int align, int zero)
{
    puti_color(n, BLACK, WHITE, align, zero);
}

void putl_color(long n, uint32 BackColor, uint32 ForeColor, int align, int zero)
{
    int cnt = 0, i;
    char num[25];
    long tmp_n = n;
    n = n < 0 ? -n : n;

    while (n / 10)
    {
        num[cnt++] = '0' + n % 10;
        n /= 10;
    }
    num[cnt++] = '0' + n % 10;
    while(align > cnt)
        if (zero)
            num[cnt++] = '0';
        else
            num[cnt++] = ' ';
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

void putl(long n, int align, int zero)
{
    putl_color(n, BLACK, WHITE, align, zero);
}

void putx_color(long n, uint32 BackColor, uint32 ForeColor, int isuper, int align, int zero)
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
    while(align > cnt)
        if (zero)
            num[cnt++] = '0';
        else
            num[cnt++] = ' ';
        
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

void putx(long n, int isuper, int align, int zero)
{
    putx_color(n, BLACK, WHITE, isuper, align, zero);
}

void putui_color(unsigned int n, uint32 BackColor, uint32 ForeColor, int align, int zero)
{
    unsigned int cnt = 0, i;
    char num[25];

    while (n / 10)
    {
        num[cnt++] = '0' + n % 10;
        n /= 10;
    }
    num[cnt++] = '0' + n % 10;
    
    while(align > cnt)
        if (zero)
            num[cnt++] = '0';
        else
            num[cnt++] = ' ';

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

void putui(unsigned int n, int align, int zero)
{
    putui_color(n, BLACK, WHITE, align, zero);
}

void putul_color(unsigned long n, uint32 BackColor, uint32 ForeColor, int align, int zero)
{
    unsigned int cnt = 0, i;
    char num[25];

    while (n / 10)
    {
        num[cnt++] = '0' + n % 10;
        n /= 10;
    }
    num[cnt++] = '0' + n % 10;
    while(align > cnt)
        if (zero)
            num[cnt++] = '0';
        else
            num[cnt++] = ' ';

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

void putul(unsigned long n, int align, int zero)
{
    putul_color(n, BLACK, WHITE, align, zero);
}

void putux_color(unsigned long n, uint32 BackColor, uint32 ForeColor, int isuper, int align, int zero)
{
    unsigned int cnt = 0, i;
    char num[25];

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
    while(align > cnt)
        if (zero)
            num[cnt++] = '0';
        else
            num[cnt++] = ' ';
        
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

void putux(unsigned long n, int isuper, int align, int zero)
{
    putux_color(n, BLACK, WHITE, isuper, align, zero);
}

void memset(void *str, unsigned char c, long size)
{
    unsigned char * s = (unsigned char *)str;
    long i = 0;
    while(i<size)
        *(s + i++) = c;
}
