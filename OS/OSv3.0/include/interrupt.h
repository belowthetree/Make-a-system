#ifndef INTERRUPT_H
#define INTERRUPT_H


#define PIC1_OCW2		0x00a0
#define PIC0_OCW2		0x0020

void InitInterrupt();

void do_IRQ();

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

void system_call_function();



















#endif