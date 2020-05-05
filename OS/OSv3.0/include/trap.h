#ifndef TRAP_H
#define TRAP_H

void InitTrap();

void SystemTrap(unsigned long rsp, unsigned long error_code);


// 错误入口函数
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


















#endif