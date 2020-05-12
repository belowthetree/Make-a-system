#include "timer.h"
#include "descriptor.h"
#include "lib.h"
#include "io.h"
#include "interrupt.h"
#include "memory.h"

#define READ_CMOS(addr) ({\
	io_out8(0x70, 0x80 | addr);\
	io_in8(0x71);\
})

TIME time;

void initHPET();

void InitTimer(){
	cli();

	do{
		time.second = READ_CMOS(0x00);
		time.minute = READ_CMOS(0x02);
		time.hour = READ_CMOS(0x04);
		time.day = READ_CMOS(0x07);
		time.month = READ_CMOS(0x08);
		time.year = READ_CMOS(0x09) + READ_CMOS(0x32) * 0x100;

	}while(time.second != READ_CMOS(0x00));

	printf_color(BLACK, RED, "%x %x %x %x %x\n", 
		time.year, time.month, time.day, time.hour, time.minute, time.second);
	io_out8(0x70, 0);
	sti();

	// initHPET();
	// PIT();
}

void get_time(TIME* time){
	cli();

	do{
		time->second = READ_CMOS(0x00);
		time->minute = READ_CMOS(0x02);
		time->hour = READ_CMOS(0x04);
		time->day = READ_CMOS(0x07);
		time->month = READ_CMOS(0x08);
		time->year = READ_CMOS(0x09) + READ_CMOS(0x32) * 0x100;

	}while(time->second != READ_CMOS(0x00));

	io_out8(0x70, 0);
	sti();
}

void Timer(unsigned long regs){
	printf_color(BLACK, GREEN, "tik regs at %ux\t", regs);
	while(1);
}

void PIT(){
	printf_color(BLACK, GREEN, "Set\n");
	unsigned short frequency = 0xffffffff;

	io_out8(0x43, 0x36);
	io_out8(0x40, ((unsigned char)frequency & 0xff));
	io_out8(0x40, ((unsigned char)(frequency >> 8) & 0xff));
	register_irq(0x20, Timer);
}

void HPETHandler(){
	printf_color(BLACK, GREEN, "tik tok\n");
}

void initHPET(){
	register_irq(0x22, HPETHandler);

	unsigned int x;
	unsigned int * p;

	unsigned char * addr = (unsigned char *)0xffff800001100000;

	io_out32(0xcf8, 0x8000f8f0);
	x = io_in32(0xcfc);
	
	printf_color(BLACK, RED, "%ux\n", x);
	x = x & 0xffffc000;
	p = 0xffff8000015fc000;
	printf_color(BLACK, RED, "%ux\n", x);

	if (x > 0xfec00000  && x < 0xfee00000) {
		p = (unsigned int *)(x + 0x3404UL);
	}

	*p = 0x80;
	io_mfence();
	
	*(unsigned int *)(addr + 0x10) = 3;
	printf_color(BLACK, RED, "hee\n");
	io_mfence();

	*(unsigned int *)(addr + 0x100) = 0x004c;
	io_mfence();

	// 间隔 1S
	*(unsigned int *)(addr + 0x108) = 14318179;
	io_mfence();

	// MAIN_CNT
	*(unsigned int *)(addr + 0xf0) = 0;
	io_mfence();
}