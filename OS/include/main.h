#ifndef _MAIN_H
#define _MAIN_H
#define BLACK       0x0
#define DEEP_BLUE   0x1
#define DEEP_GREEN  0x2
#define MIDDLE_BLUE 0x3
#define DEEP_RED    0x4
#define DEEP_PURPLE 0x5
#define BROUND      0x6
#define LIGHT_PURPLE 0xd
#define GOLD        0xe
#define WHITE       0xf

#define VRAM        0xb8000

// kernel.asm
void io_hlt();
void io_sti();
void io_out8(int port, int data);
void io_cli();
void io_delay();
void load_idtr(int limit, int addr);
void load_gdtr(int limit, int addr);
int io_load_eflags();
void io_store_eflags(int eflags);
void register_clock();
void create_int();

// graph.c
void memcpy(unsigned str, unsigned value, unsigned len);
void SetVRAM(int x, int y, char c, char background, char color);
void InitScreen();
void InitPallete();

// init.c
void WriteMem8(char *addr, char value);


// main.c

#define DA_386IGate	  0x8E	// 386 中断门类型值
void Clock();


#endif