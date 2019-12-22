#ifndef	_ABNORMAL_H
#define	_ABNORMAL_H
#include "type.h"

#define _set_gate(gate_selector_addr,attr,ist,code_addr)	\
do								\
{	unsigned long __d0,__d1;				\
	__asm__ __volatile__	(	"movw	%%dx,	%%ax	\n\t"	\
					"andq	$0x7,	%%rcx	\n\t"	\
					"addl	%4,	%%ecx	\n\t"	\
					"shlq	$32,	%%rcx	\n\t"	\
					"addq	%%rcx,	%%rax	\n\t"	\
					"xorq	%%rcx,	%%rcx	\n\t"	\
					"movl	%%edx,	%%ecx	\n\t"	\
					"shrq	$16,	%%rcx	\n\t"	\
					"shlq	$48,	%%rcx	\n\t"	\
					"addq	%%rcx,	%%rax	\n\t"	\
					"movq	%%rax,	%0	\n\t"	\
					"shrq	$32,	%%rdx	\n\t"	\
					"movq	%%rdx,	%1	\n\t"	\
					:"=m"(*((unsigned long *)(gate_selector_addr)))	,					\
					 "=m"(*(1 + (unsigned long *)(gate_selector_addr))),"=&a"(__d0),"=&d"(__d1)		\
					:"i"(attr << 8),									\
					 "3"((unsigned long *)(code_addr)),"2"(0x8 << 16),"c"(ist)				\
					:"memory"		\
				);				\
}while(0)

#define load_TR(n) 							\
do{									\
	__asm__ __volatile__(	"ltr	%%ax"				\
				:					\
				:"a"(n << 3)				\
				:"memory");				\
}while(0)

struct desc_struct 
{
	unsigned char x[8];
};

struct gate_struct
{
	unsigned char x[16];
};

extern struct desc_struct GDT_Table[];
extern struct gate_struct IDT_Table[];
extern unsigned int TSS64_Table[26];
// 门初始化
void set_intr_gate(uint32 n, uint8 ist, void * addr);
void set_trap_gate(uint32 n, uint8 ist, void * addr);
void set_system_gate(uint32 n, uint8 ist, void * addr);

// 写入门
void set_gate(uint8 * addr, uint8 attr, uint8 ist, void * func);
// 系统级中断向量初始化
void sys_vector_init();
// TSS初始化
void set_tss64(unsigned long rsp0,unsigned long rsp1,unsigned long rsp2,unsigned long ist1,unsigned long ist2,unsigned long ist3,
unsigned long ist4,unsigned long ist5,unsigned long ist6,unsigned long ist7);

// 中断中转函数
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