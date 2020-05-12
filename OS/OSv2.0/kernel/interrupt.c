#include "interrupt.h"
#include "graph.h"
#include "io.h"

void init_interrupt()
{
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

	printf_color(BLACK, RED, "8259A init \n");

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
	io_out8(0x21,0x01);
	io_out8(0xa1,0x00);
	// 此处会令系统重启，慎用
	// io_out8(0x64, 0xfe);

	sti();
}

void register_irq(int num_irq, void * handler){
	interrupt[num_irq - 32] = handler;
}

void Timer()
{
	static int clock = 0;
	printf("Timer clock: %d\t", clock++);
}

void do_IRQ(unsigned long regs, unsigned long nr)
{
	if (nr == 0x20)
	{
		// Timer();
		;
	}
	else if (interrupt[nr]){
		interrupt[nr]();
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
}

void io_out32(unsigned short port,unsigned int value)
{
	__asm__ __volatile__(	"outl	%0,	%%dx	\n\t"
				"mfence			\n\t"
				:
				:"a"(value),"d"(port)
				:"memory");
}

void io_out8(unsigned short port,unsigned char value)
{
	__asm__ __volatile__(	"outb	%0,	%%dx	\n\t"
				"mfence			\n\t"
				:
				:"a"(value),"d"(port)
				:"memory");
}

unsigned char io_in8(unsigned short port)
{
	unsigned char ret = 0;
	__asm__ __volatile__(	"inb	%%dx,	%0	\n\t"
				"mfence			\n\t"
				:"=a"(ret)
				:"d"(port)
				:"memory");
	return ret;
}
unsigned int io_in32(unsigned short port)
{
	unsigned int ret = 0;
	__asm__ __volatile__(	"inl	%%dx,	%0	\n\t"
				"mfence			\n\t"
				:"=a"(ret)
				:"d"(port)
				:"memory");
	return ret;
}

// void IRQ0x20_interrupt()
// {
// 	__asm__ (	"IRQ_0x20interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x20,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }

// void IRQ0x21_interrupt()
// {
// 	__asm__ (	"IRQ_0x21interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x21,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x22_interrupt()
// {
// 	__asm__ (	"IRQ_0x22interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x22,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x23_interrupt()
// {
// 	__asm__ (	"IRQ_0x23interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x23,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x24_interrupt()
// {
// 	__asm__ (	"IRQ_0x24interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x24,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x25_interrupt()
// {
// 	__asm__ (	"IRQ_0x25interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x25,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x26_interrupt()
// {
// 	__asm__ (	"IRQ_0x26interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x26,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x27_interrupt()
// {
// 	__asm__ (	"IRQ_0x27interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x27,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x28_interrupt()
// {
// 	__asm__ (	"IRQ_0x28interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x28,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x29_interrupt()
// {
// 	__asm__ (	"IRQ_0x29interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x29,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x2a_interrupt()
// {
// 	__asm__ (	"IRQ_0x2ainterrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x2a,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x2b_interrupt()
// {
// 	__asm__ (	"IRQ_0x2binterrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x2b,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x2c_interrupt()
// {
// 	__asm__ (	"IRQ_0x2cinterrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x2c,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x2d_interrupt()
// {
// 	__asm__ (	"IRQ_0x2dinterrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x2d,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x2e_interrupt()
// {
// 	__asm__ (	"IRQ_0x2einterrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x2e,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x2f_interrupt()
// {
// 	__asm__ (	"IRQ_0x2finterrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x2f,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x30_interrupt()
// {
// 	__asm__ (	"IRQ_0x30interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x30,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x31_interrupt()
// {
// 	__asm__ (	"IRQ_0x31interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x31,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x32_interrupt()
// {
// 	__asm__ (	"IRQ_0x32interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x32,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x33_interrupt()
// {
// 	__asm__ (	"IRQ_0x33interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x33,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x34_interrupt()
// {
// 	__asm__ (	"IRQ_0x34interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x34,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x35_interrupt()
// {
// 	__asm__ (	"IRQ_0x35interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x35,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x36_interrupt()
// {
// 	__asm__ (	"IRQ_0x36interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x36,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }
// void IRQ0x37_interrupt()
// {
// 	__asm__ (	"IRQ_0x37interrupt:		\n\t"	\
// 				"pushq	$0x00				\n\t"	\
// 				SAVE_ALL					\
// 				"movq	%rsp,	%rdi			\n\t"	\
// 				"leaq	ret_from_intr(%rip),	%rax	\n\t"	\
// 				"pushq	%rax				\n\t"	\
// 				"movq	$0x37,	%rsi			\n\t"	\
// 				"jmp	do_IRQ	\n\t");
// }