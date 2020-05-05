#include "interrupt.h"
#include "io.h"
#include "lib.h"


void (* interrupt[24])();
void (* do_int[0x24])();

void Timer();

void InitInterrupt()
{
	printf_color(BLACK, INDIGO, "init interrupt\n");
	interrupt[0] = IRQ0x20_interrupt;
	interrupt[1] = IRQ0x21_interrupt;
	interrupt[2] = IRQ0x22_interrupt;
	interrupt[3] = IRQ0x23_interrupt;
	interrupt[4] = IRQ0x24_interrupt;
	interrupt[5] = IRQ0x25_interrupt;
	interrupt[6] = IRQ0x26_interrupt;
	interrupt[7] = IRQ0x27_interrupt;
	interrupt[8] = IRQ0x28_interrupt;
	interrupt[9] = IRQ0x29_interrupt;
	interrupt[10] = IRQ0x2a_interrupt;
	interrupt[11] = IRQ0x2b_interrupt;
	interrupt[12] = IRQ0x2c_interrupt;
	interrupt[13] = IRQ0x2d_interrupt;
	interrupt[14] = IRQ0x2e_interrupt;
	interrupt[15] = IRQ0x2f_interrupt;
	interrupt[16] = IRQ0x30_interrupt;
	interrupt[17] = IRQ0x31_interrupt;
	interrupt[18] = IRQ0x32_interrupt;
	interrupt[19] = IRQ0x33_interrupt;
	interrupt[20] = IRQ0x34_interrupt;
	interrupt[21] = IRQ0x35_interrupt;
	interrupt[22] = IRQ0x36_interrupt;
	interrupt[23] = IRQ0x37_interrupt;
	int i;
	for(i = 32;i < 56;i++)
		set_intr_gate(i , 2 , interrupt[i - 32]);

	// do_int[0] = Timer;
//	printf_color(BLACK, RED, "8259A init \n");

	//8259A-master	ICW1-4
	io_out8(0x20,0x11);
	io_out8(0x21,0x20);
	io_out8(0x21,0x04);
	io_out8(0x21,0x01);

	//8259A-slave	ICW1-4
	io_out8(0xa0,0x11);
	io_out8(0xa1,0x28);
	io_out8(0xa1,0x02);
	io_out8(0xa1,0x01);

	//只开启键盘中断|全部开启
	io_out8(0x21,0xff);
	io_out8(0xa1,0xff);
	// 此处会令系统重启，慎用
	// io_out8(0x64, 0xfe);

	sti();
}

void register_irq(int num_irq, void * handler){
	do_int[num_irq] = handler;
}

void Timer()
{
	static int clock = 0;
	printf("Timer clock: %d\t", clock++);
}

void do_IRQ(unsigned long regs, unsigned long nr)
{
	nr -= 0x20;
	if (do_int[nr]){
		do_int[nr]();
	}
	else
	{
		unsigned char x;
		printf_color(BLACK, RED, "do_IRQ:%08X\n", nr);
		x = io_in8(0x60);
		printf_color(BLACK, RED, "key code:%018X\n",x);
	}
	io_out8(PIC1_OCW2, 0x64);	/* 通知PIC IRQ-12 已经受理完毕 */
	io_out8(PIC0_OCW2, 0x62);	/* 通知PIC IRQ-02 已经受理完毕 */
	io_out8(0x20,0x20);
	printf("exit\n");
}

