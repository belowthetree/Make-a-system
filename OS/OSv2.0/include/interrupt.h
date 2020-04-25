#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#define sti() 		__asm__ __volatile__ ("sti	\n\t":::"memory")
#define SAVE_ALL				\
	"cld;			\n\t"		\
	"pushq	%rax;		\n\t"		\
	"movq	%es,	%rax;	\n\t"		\
	"pushq	%rax;		\n\t"		\
	"movq	%ds,	%rax;	\n\t"		\
	"pushq	%rax;		\n\t"		\
	"xorq	%rax,	%rax;	\n\t"		\
	"pushq	%rbp;		\n\t"		\
	"pushq	%rdi;		\n\t"		\
	"pushq	%rsi;		\n\t"		\
	"pushq	%rdx;		\n\t"		\
	"pushq	%rcx;		\n\t"		\
	"pushq	%rbx;		\n\t"		\
	"pushq	%r8;		\n\t"		\
	"pushq	%r9;		\n\t"		\
	"pushq	%r10;		\n\t"		\
	"pushq	%r11;		\n\t"		\
	"pushq	%r12;		\n\t"		\
	"pushq	%r13;		\n\t"		\
	"pushq	%r14;		\n\t"		\
	"pushq	%r15;		\n\t"		\
	"movq	$0x10,	%rdx;	\n\t"		\
	"movq	%rdx,	%ds;	\n\t"		\
	"movq	%rdx,	%es;	\n\t"


void IRQ0x20_interrupt();
void IRQ0x21_interrupt();
void IRQ0x22_interrupt();
void IRQ0x23_interrupt();
void IRQ0x24_interrupt();
void IRQ0x25_interrupt();
void IRQ0x26_interrupt();
void IRQ0x27_interrupt();
void IRQ0x28_interrupt();
void IRQ0x29_interrupt();
void IRQ0x2a_interrupt();
void IRQ0x2b_interrupt();
void IRQ0x2c_interrupt();
void IRQ0x2d_interrupt();
void IRQ0x2e_interrupt();
void IRQ0x2f_interrupt();
void IRQ0x30_interrupt();
void IRQ0x31_interrupt();
void IRQ0x32_interrupt();
void IRQ0x33_interrupt();
void IRQ0x34_interrupt();
void IRQ0x35_interrupt();
void IRQ0x36_interrupt();
void IRQ0x37_interrupt();
void IRQ0x38_interrupt();
void IRQ0x39_interrupt();
void IRQ0x3a_interrupt();
void IRQ0x3b_interrupt();
void IRQ0x3c_interrupt();
void IRQ0x3d_interrupt();
void IRQ0x3e_interrupt();
void IRQ0x3f_interrupt();

void (* interrupt[24])(void);

void Timer();

void init_interrupt();
void do_IRQ(unsigned long regs, unsigned long nr);
void register_irq(int num_irq, void * handler);

void io_out8(unsigned short port,unsigned char value);
void io_out32(unsigned short port,unsigned int value);

unsigned char io_in8(unsigned short port);
unsigned int io_in32(unsigned short port);


#endif