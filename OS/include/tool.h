#ifndef TOOL_H
#define TOOL_H

int font_x, font_y;

void prints(char str []);
void printi(int data, int y);
void printf(const char *str, ...);

unsigned int getmem(unsigned int start, unsigned int end);
unsigned int getmem_sub(unsigned int start, unsigned int end);

void memset(char *str, char value, unsigned int size);

#endif