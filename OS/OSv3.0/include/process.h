#ifndef PROCESS_H
#define PROCESS_H
#include "descriptor.h"
#include "timer.h"

#define TASKSTACKSIZE 4096

#define KERNELCODE 0x8UL
#define KERNELDATA 0x10UL

struct TASK* Task;

typedef struct {
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
	unsigned long rax;//0x80
	unsigned long rip;
	unsigned long cs;
	unsigned long rflags;
	unsigned long rsp;
	unsigned long ss;

	unsigned long rsp0;
	unsigned long error_code;

	struct TASK* task;
} PCB;

struct TASK {
	unsigned long pid;
	unsigned long addr_limit;
	TIME* time;
	long priority;
	PCB* pcb;
	struct TASK* prev, *next;
} __attribute__((packed));

void (* sys_func[128])();

void system_call();

void InitProcess();
void switch_to(PCB* pcb, PCB* current, PCB* next);

// struct mm_struct{
// 	pml4t_t *pgd;	// 页表指针

// 	unsigned long start_code, end_code;		// 代码段
// 	unsigned long start_data, end_data;		// 数据段
// 	unsigned long start_rodata, end_rodata;	// 只读数据段
// 	unsigned long start_brk, end_brk;		// 动态内存分配区
// 	unsigned long start_stack;				// 应用层栈基地址
// };

























#endif