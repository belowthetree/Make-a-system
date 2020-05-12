#include "process.h"
#include "io.h"
#include "interrupt.h"
#include "memory.h"

unsigned long pid = 0;

void process1();
void process2();
void print();

void schedule(unsigned long regs);

void InitProcess(){
	Task = 0;
	printf_color(BLACK, YELLOW, "process\n");
	sys_func[0] = print;
	create_process(0);
	create_process(process1);
	create_process(process2);

	// printf_color(BLACK, GREEN, "rip %ux, cs %ux\n", Task->pcb->rip, Task->pcb->cs);
	// printf_color(BLACK, RED, "%x %x %x %x %x\n", 
	// 	Task->time->year, Task->time->month, Task->time->day, Task->time->hour, Task->time->minute, Task->time->second);
	// printf_color(BLACK, GREEN, "rip %ux, cs %ux\n", Task->next->pcb->rip, Task->next->pcb->cs);
	// printf_color(BLACK, RED, "%x %x %x %x %x\n", 
	// 	Task->next->time->year, Task->next->time->month, Task->next->time->day, Task->next->time->hour, 
	// 	Task->next->time->minute, Task->next->time->second);

	register_irq(0x20, schedule);
}

void print(){
	printf("System call\n");
}

void system(PCB* pcb, unsigned long sys_num){
	printf_color(BLACK, RED, "cs %ux\n", pcb->cs);
	wrmsr(0x174, 0x08);
	wrmsr(0x175, pcb->rsp0);
	wrmsr(0x176, (unsigned long)system_call);

	__asm__ __volatile__(
		"leaq ret_addr(%%rip), %%rdx\n\t"
		"movq %%rsp, %%rcx\n\t"
		"sysenter\n\t"
		"ret_addr:\n\t"
		:
		:"a"(sys_num));
}

void do_system_call(PCB* regs){
	if (sys_func[regs->rax])
		sys_func[regs->rax]();
	else
		printf_color(BLACK, RED, "No system call for num %ux\n", regs->rax);
	// wrmsr(0x174, 0x08);
}

void create_process(void * func){
	struct TASK* task = kmalloc(sizeof(struct TASK));

	task->pcb = kmalloc(TASKSTACKSIZE) + TASKSTACKSIZE - sizeof(PCB);
	task->pcb->rip = (unsigned long)func;
	task->pcb->cs = KERNELCODE;
	task->pcb->ds = KERNELDATA;
	task->pcb->es  = KERNELDATA;
	task->pcb->ss = KERNELDATA;
	task->pcb->rsp = task->pcb;
	task->pid = pid++;
	task->pcb->rflags = 0x200UL;
	task->pcb->task = task;
	task->time = kmalloc(sizeof(TIME));
	task->pcb->rsp0 = kmalloc(sizeof(TASKSTACKSIZE) + TASKSTACKSIZE);

	get_time(task->time);

	add_task(task);
}

void schedule(unsigned long regs){
	if (Task->next != Task){
		switch_to(regs, Task->pcb, Task->next->pcb);
		Task = Task->next;
	}
}

void process1(){
	while(1){
		printf_color(BLACK, RED, "Process1\t");
	}
}

void process2(){
	while(1){
		printf_color(BLACK, RED, "Process2\t");
	}
}

void add_task(struct TASK* task){
	if (Task == 0){
		Task = task;
		Task->next = Task;
		Task->prev = Task;
		return;
	}

	task->next = Task->next;
	Task->next = task;
	task->prev = Task;
	task->next->prev = task;
}
