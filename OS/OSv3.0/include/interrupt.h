#ifndef INTERRUPT_H
#define INTERRUPT_H


#define PIC1_OCW2		0x00a0
#define PIC0_OCW2		0x0020

void InitInterrupt();

void do_IRQ();

// 系统异常入口函数
void divide_error();
void machine_check();
void debug();
void nmi();
void int3();
void overflow();
void bounds();
void undefined_opcode();
void dev_not_available();
void double_fault();
void coprocessor_segment_overrun();
void invalid_TSS();
void segment_not_present();
void stack_segment_fault();
void page_fault();
void x87_FPU_error();
void alignment_check();
void machine_check();
void SIMD_exception();
void virtualization_exception();
void general_protection();

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





















#endif