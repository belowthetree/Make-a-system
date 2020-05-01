#ifndef ABNORMAL_H
#define ABNORMAL_H
#include "type.h"

#define INTVECBASE 0xffff800000040000
#define INTENTERBASE 0xffff800000041000

struct GATE{
	unsigned char cx[16];
};

struct POINTER{
	unsigned int limit;
	unsigned long* addr;
}IDT_Pointer;

struct GATE* IDT_Table;

void InitAbnormal();
void LIDT(unsigned long* addr);



void SolveErrorInt(unsigned long rsp, unsigned long error_code);

void do_invalid_TSS(uint64 rsp, uint64 error_code);
void do_nmi(unsigned long rsp,unsigned long error_code);
void do_double_fault(unsigned long rsp,unsigned long error_code);
void do_segment_not_present(unsigned long rsp,unsigned long error_code);
void do_coprocessor_segment_overrun(unsigned long rsp,unsigned long error_code);
void do_general_protection(unsigned long rsp,unsigned long error_code);


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