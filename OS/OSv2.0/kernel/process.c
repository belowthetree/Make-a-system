#include "process.h"
#include "io.h"

void init_tsk()
{
	init_task_union.task.state = TASK_UNINTERRUPTIBLE;
	init_task_union.task.flags = PF_KTHREAD;
	init_task_union.task.mm = &init_mm;
	init_task_union.task.thread = &init_thread;
	init_task_union.task.addr_limit = 0xffff800000000000;
	init_task_union.task.pid = 0;
	init_task_union.task.counter = 1;
	init_task_union.task.signal = 0;
	init_task_union.task.priority = 0;
}

unsigned long init(unsigned long arg)
{
	printf_color(BLACK, RED, "init task is running,arg:%#018lx\n",arg);

	return 1;
}

void init_process()
{
	struct task_struct *p = NULL;

	init_mm.pgd = (pml4t_t *)Global_CR3;
	init_mm.start_code = memory_management_struct.start_code;
	init_mm.end_code = memory_management_struct.end_code;
	init_mm.start_data = (unsigned long)&_data;
	init_mm.end_data = memory_management_struct.end_data;
	init_mm.start_rodata = (unsigned long)&_rodata; 
	init_mm.end_rodata = (unsigned long)&_erodata;
	init_mm.start_brk = 0;
	init_mm.end_brk = memory_management_struct.end_brk;
	init_mm.start_stack = _stack_start;
	
	wrmsr(0x174,KERNEL_CS);

//	init_thread,init_tss
	set_tss64(init_thread.rsp0, init_tss[0].rsp1, init_tss[0].rsp2, init_tss[0].ist1, init_tss[0].ist2, init_tss[0].ist3, init_tss[0].ist4, init_tss[0].ist5, init_tss[0].ist6, init_tss[0].ist7);
	init_tss[0].rsp0 = init_thread.rsp0;
	list_init(&init_task_union.task.list);
	kernel_thread(init,10,CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	init_task_union.task.state = TASK_RUNNING;
	p = container_of(list_next(&current->list),struct task_struct,list);

	switch_to(current,p);
}

struct task_struct * get_current()
{
	struct task_struct * cur = NULL;
	__asm__ __volatile__ ("andq %%rsp,%0	\n\t":"=r"(cur):"0"(~32767UL));
	return cur;
}

void __switch_to(struct task_struct *prev,struct task_struct *next)
{
	init_tss[0].rsp0 = next->thread->rsp0;
	set_tss64(init_tss[0].rsp0, init_tss[0].rsp1, init_tss[0].rsp2, init_tss[0].ist1, init_tss[0].ist2, init_tss[0].ist3, init_tss[0].ist4, init_tss[0].ist5, init_tss[0].ist6, init_tss[0].ist7);

	__asm__ __volatile__("movq	%%fs,	%0 \n\t":"=a"(prev->thread->fs));
	__asm__ __volatile__("movq	%%gs,	%0 \n\t":"=a"(prev->thread->gs));

	__asm__ __volatile__("movq	%0,	%%fs \n\t"::"a"(next->thread->fs));
	__asm__ __volatile__("movq	%0,	%%gs \n\t"::"a"(next->thread->gs));

	printf_color(BLACK, WHITE, "prev->thread->rsp0:%018X\n",prev->thread->rsp0);
	printf_color(BLACK, WHITE, "next->thread->rsp0:%018X\n",next->thread->rsp0);
}

int kernel_thread(unsigned long (* fn)(unsigned long), unsigned long arg, unsigned long flags)
{
	struct pt_regs regs;
	memset((unsigned char*)&regs,0,sizeof(regs));

	regs.rbx = (unsigned long)fn;
	regs.rdx = (unsigned long)arg;

	regs.ds = KERNEL_DS;
	regs.es = KERNEL_DS;
	regs.cs = KERNEL_CS;
	regs.ss = KERNEL_DS;
	regs.rflags = (1 << 9);
	regs.rip = (unsigned long)kernel_thread_func;

	return do_fork(&regs,flags,0,0);
}