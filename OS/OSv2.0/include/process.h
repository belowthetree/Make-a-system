#ifndef _PROCESS_H
#define _PROCESS_H
#include "memory.h"
#include "type.h"
#include "graph.h"
#include "cpu.h"
#include "lib.h"

#define TASK_RUNNING		(1 << 0)
#define TASK_INTERRUPTIBLE	(1 << 1)
#define	TASK_UNINTERRUPTIBLE	(1 << 2)
#define	TASK_ZOMBIE		(1 << 3)	
#define	TASK_STOPPED		(1 << 4)
#define PF_KTHREAD	(1 << 0)

#define KERNEL_CS 	(0x08)	// 内核代码段选择子
#define	KERNEL_DS 	(0x10)	// 内核数据段选择子
#define	USER_CS		(0x28)	// 用户代码段选择子
#define USER_DS		(0x30)	// 用户数据段选择子

#define CLONE_FS	(1 << 0)
#define CLONE_FILES	(1 << 1)
#define CLONE_SIGNAL	(1 << 2)

// stack size 32K
#define STACK_SIZE 32768

#define GET_CURRENT			\
	"movq	%rsp,	%rbx	\n\t"	\
	"andq	$-32768,%rbx	\n\t"

extern char _text;
extern char _etext;
extern char _data;
extern char _edata;
extern char _rodata;
extern char _erodata;
extern char _bss;
extern char _ebss;
extern char _end;

extern unsigned long _stack_start;

typedef struct task_struct{
	struct List list;				// 双向链表，连接各个 PCB
	volatile long state;			// 进程状态：运行、停止、可中断
	unsigned long flags;			// 进程标志：进程、线程、内核线程

	struct mm_struct *mm;			// 内存空间分布结构体，记录内存页表和程序段信息
	struct thread_struct *thread;	// 进程切换时博爱刘的状态信息

	unsigned long addr_limit;		// 进程地址空间范围
							/*0x0000,0000,0000,0000 - 0x0000,7fff,ffff,ffff user*/
							/*0xffff,8000,0000,0000 - 0xffff,ffff,ffff,ffff kernel*/
	long pid;						// 进程ID
	long counter;					// 剩余时间片
	long signal;					// 进程持有的信号
	long priority;					// 进程优先级
}PCB;
// 内存信息
struct mm_struct{
	pml4t_t *pgd;	// 页表指针

	unsigned long start_code, end_code;		// 代码段
	unsigned long start_data, end_data;		// 数据段
	unsigned long start_rodata, end_rodata;	// 只读数据段
	unsigned long start_brk, end_brk;		// 动态内存分配区
	unsigned long start_stack;				// 应用层栈基地址
};
// 保存进程现场，似乎并不保存通用寄存器
struct thread_struct{
	unsigned long rsp0;			// 内核层栈基地址

	unsigned long rip;			// 内核层代码指针
	unsigned long rsp;			// 内核层当前栈指针

	unsigned long fs;			// FS 段寄存器
	unsigned long gs;			// GS 段寄存器

	unsigned long cr2;			// CR2控制寄存器
	unsigned long trap_nr;		// 产生异常的异常号
	unsigned long error_code;	// 异常的错误码
};
// 进程区域和栈内核栈区域合并
union task_union{
	struct task_struct task;
	unsigned long stack[STACK_SIZE / sizeof(unsigned long)];
}__attribute__((aligned(8)));

// 链接到 data.init_task 段内
union task_union init_task_union __attribute__((__section__ (".data.init_task")));
// = {INIT_TASK(init_task_union.task)};
// 支持 8 个进程，第一个进程为 init_task_union
struct task_struct *init_task[NR_CPUS] = {&init_task_union.task,0};

struct mm_struct init_mm = {0};
// 初始进程
struct thread_struct init_thread = 
{
	.rsp0 = (unsigned long)(init_task_union.stack + STACK_SIZE / sizeof(unsigned long)),
	.rsp = (unsigned long)(init_task_union.stack + STACK_SIZE / sizeof(unsigned long)),
	.fs = KERNEL_DS,
	.gs = KERNEL_DS,
	.cr2 = 0,
	.trap_nr = 0,
	.error_code = 0
};

struct tss_struct
{
	unsigned int  reserved0;
	unsigned long rsp0;
	unsigned long rsp1;
	unsigned long rsp2;
	unsigned long reserved1;
	unsigned long ist1;
	unsigned long ist2;
	unsigned long ist3;
	unsigned long ist4;
	unsigned long ist5;
	unsigned long ist6;
	unsigned long ist7;
	unsigned long reserved2;
	unsigned short reserved3;
	unsigned short iomapbaseaddr;
}__attribute__((packed));

#define INIT_TSS \
{	0,	 \
	(unsigned long)(init_task_union.stack + STACK_SIZE / sizeof(unsigned long)),	\
	(unsigned long)(init_task_union.stack + STACK_SIZE / sizeof(unsigned long)),	\
	(unsigned long)(init_task_union.stack + STACK_SIZE / sizeof(unsigned long)),	\
	0,	 \
	0xffff800000007c00,	\
	0xffff800000007c00,	\
	0xffff800000007c00,	\
	0xffff800000007c00,	\
	0xffff800000007c00,	\
	0xffff800000007c00,	\
	0xffff800000007c00,	\
	0,	\
	0,	\
	0	\
}
// 一个 CPU 一个 TSS？
struct tss_struct init_tss[NR_CPUS] = {INIT_TSS};
// 现场结构体
struct pt_regs
{
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long rbx;
	unsigned long rcx;
	unsigned long rdx;
	unsigned long rsi;
	unsigned long rdi;
	unsigned long rbp;
	unsigned long ds;
	unsigned long es;
	unsigned long rax;
	unsigned long func;
	unsigned long errcode;
	unsigned long rip;
	unsigned long cs;
	unsigned long rflags;
	unsigned long rsp;
	unsigned long ss;
};

unsigned long do_fork(
	struct pt_regs * regs,
	unsigned long clone_flags,
	unsigned long stack_start,
	unsigned long stack_size
);
void init_process();

struct task_struct * get_current();

#define switch_to(prev,next)			\
do{							\
	__asm__ __volatile__ (	"pushq	%%rbp	\n\t"	\
				"pushq	%%rax	\n\t"	\
				"movq	%%rsp,	%0	\n\t"	\
				"movq	%2,	%%rsp	\n\t"	\
				"leaq	1f(%%rip),	%%rax	\n\t"	\
				"movq	%%rax,	%1	\n\t"	\
				"pushq	%3		\n\t"	\
				"jmp	__switch_to	\n\t"	\
				"1:	\n\t"	\
				"popq	%%rax	\n\t"	\
				"popq	%%rbp	\n\t"	\
				:"=m"(prev->thread->rsp),"=m"(prev->thread->rip)		\
				:"m"(next->thread->rsp),"m"(next->thread->rip),"D"(prev),"S"(next)	\
				:"memory"		\
				);			\
}while(0)

extern void kernel_thread_func(void);
void __switch_to(struct task_struct *prev,struct task_struct *next);

#define current get_current()




#endif