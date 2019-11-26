#include "type.h"
#ifndef _IO_H
#define _IO_H

void putchar(char c);
void putchar_color(char c, uint32 BackColor, uint32 ForeColor);
void puts(char *str);
void puts_color(char *str, uint32 BackColor, uint32 ForeColor);
void printf(char *str, ...);
void printf_color(uint32 BackColor, uint32 ForeColor, char *str, ...);
void puti(int n);
void puti_color(int n, uint32 BackColor, uint32 ForeColor);
void putx(int n, int isuper);
void putx_color(int n, uint32 BackColor, uint32 ForeColor, int isuper);




#endif
