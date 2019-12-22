#include "process.h"
#include "io.h"

extern void ret_system_call(void);
// 在这跳转到进程，rbx保存目标进程地址
extern void kernel_thread_func(void);
extern void system_call();

void user_level_function()
{
	void (*func)() = (void *)0xffff800000105ece;
	long ret = 0;
	char string [] = "Hello world!\n";
	char msg1[] = "user_level_function task is running, printf_color%uX\n";
	func(BLACK, RED, msg1, printf_color);

	__asm__ __volatile__ ("leaq	sysexit_return_address(%%rip), %%rdx	\n\t"
						  "movq %%rsp, %%rcx	\n\t"
						  "sysenter 	\n\t"
						  "sysexit_return_address:	\n\t"
						  :"=a"(ret):"0"(1), "D"(string):"memory");

	char msg2[] = "user_level_function task called sysenter, ret:%ld\n";
	func(BLACK, RED, msg2, ret);

	while(1);
}

// 初始化第一个进程结构体
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
	struct pt_regs *regs;

	printf_color(BLACK, RED, "init task is running,arg:%018X\n",arg);
	// 通过 ret_system_call 跳转用户程序
	current->thread->rip = (unsigned long)ret_system_call;
	current->thread->rsp = (unsigned long)current + STACK_SIZE - sizeof(struct pt_regs);
	regs = (struct pt_regs *)current->thread->rsp;

	__asm__	__volatile__	(	"movq	%1,	%%rsp	\n\t"
					"pushq	%2		\n\t"
					"jmp	do_execve	\n\t"
					::"D"(regs),"m"(current->thread->rsp),"m"(current->thread->rip):"memory");

	return 1;
}

void init_process()
{
	init_tsk();
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
//  修改 IA32_SYSENTER_CS 寄存器，位于 MSR 寄存器组 0x174地址处
	wrmsr(0x174,KERNEL_CS);
	wrmsr(0x175,current->thread->rsp0);
	wrmsr(0x176,(unsigned long)system_call);
//	init_thread,init_tss
	int i;
	for (i = 1;i < NR_CPUS;i++)
		init_tss[i] = init_tss[0];
	set_tss64(init_thread.rsp0, init_tss[0].rsp1, 
		init_tss[0].rsp2, init_tss[0].ist1, init_tss[0].ist2, 
		init_tss[0].ist3, init_tss[0].ist4, init_tss[0].ist5, 
		init_tss[0].ist6, init_tss[0].ist7);

	init_tss[0].rsp0 = init_thread.rsp0;
	list_init(&init_task_union.task.list);
	kernel_thread(init,10,CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	init_task_union.task.state = TASK_RUNNING;
	p = container_of(list_next(&current->list),struct task_struct,list);
	struct task_struct * tmp = &init_task_union.task;

	switch_to(current, p);
}

struct task_struct * get_current()
{
	struct task_struct * cur = NULL;
	__asm__ __volatile__ ("andq %%rsp,%0	\n\t":"=r"(cur):"0"(~32767UL));

	return cur;
}
//
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
	// 待执行函数
	regs.rbx = (unsigned long)fn;
	// 创建者传入的参数
	regs.rdx = (unsigned long)arg;

	regs.ds = KERNEL_DS;
	regs.es = KERNEL_DS;
	regs.cs = KERNEL_CS;
	regs.ss = KERNEL_DS;
	regs.rflags = (1 << 9);
	regs.rip = (unsigned long)kernel_thread_func;

	do_fork(&regs,flags,0,0);

	return 0;
}

unsigned long do_fork(struct pt_regs * regs,
					unsigned long clone_flags,
					unsigned long stack_start,
					unsigned long stack_size)
{
	struct task_struct *tsk = NULL;
	struct thread_struct *thd = NULL;
	struct Page *p = NULL;
	// 先申请物理页
	printf("alloc_pages,bitmap:%018X\n",*memory_management_struct.bits_map);
	p = alloc_pages(ZONE_NORMAL,1,PG_PTable_Maped | PG_Active | PG_Kernel);
	printf("alloc_pages,bitmap:%018X\n",*memory_management_struct.bits_map);
	// 加上偏移 0xffff800000000000 即为进程信息保存点
	tsk = (struct task_struct *)Phy_To_Virt(p->PHY_address);
	printf("struct task_struct address:%018uX\n",(unsigned long)tsk);
	// 先置零，然后通过 rsp 将保存的 PCB 复制到 tsk
	memset(tsk,0,sizeof(*tsk));
	*tsk = *current;
	// 链接自己，
	list_init(&tsk->list);
	// 将 tsk 加到初始进程前面，进程号增加
	list_add_to_behind(&init_task_union.task.list,&tsk->list);	
	tsk->pid++;
	tsk->state = TASK_UNINTERRUPTIBLE;
	// 将进程现场放在进程 PCB 后面？
	thd = (struct thread_struct *)(tsk + 1);
	tsk->thread = thd;
	// 将执行现场复制到目标进程栈顶处
	memcpy(regs, (void *)((unsigned long)tsk + STACK_SIZE - sizeof(struct pt_regs)),sizeof(struct pt_regs));
	thd->rsp0 = (unsigned long)tsk + STACK_SIZE;
	thd->rip = regs->rip;
	thd->rsp = (unsigned long)tsk + STACK_SIZE - sizeof(struct pt_regs);
	thd->fs = KERNEL_DS;
	thd->gs = KERNEL_DS;

	if(!(tsk->flags & PF_KTHREAD))
		thd->rip = regs->rip = (unsigned long)ret_system_call;

	tsk->state = TASK_RUNNING;

	return 0;
}

unsigned long do_exit(unsigned long code)
{
	printf_color(BLACK, RED, "exit task is running,arg:%#018lx\n",code);
	while(1);
}
// 从此进入用户程序
unsigned long do_execve(struct pt_regs * regs)
{
	regs->rdx = 0x800000;	//RIP
	regs->rcx = 0xa00000;	//RSP
	regs->rax = 1;
	regs->ds = 0;
	regs->es = 0;
	printf_color(BLACK, RED, "do_execve task is running\n");
	// 将用户程序复制到 0x800000 中？
	printf("%uX\n", user_level_function);
	memcpy(user_level_function,(void *)0x800000,3*1024);

	return 0;
}

unsigned long  system_call_function(struct pt_regs * regs)
{
	return system_call_table[regs->rax](regs);
}

unsigned long no_system_call(struct pt_regs * regs)
{
	printf_color(BLACK, RED, "no_system_call is calling,NR:%04X\n",regs->rax);
	return -1;
}

unsigned long sys_printf(struct pt_regs * regs)
{
	printf_color(WHITE, BLACK, (char *)regs->rdi);
	return 1;
}

__asm__ (
".global kernel_thread_func	\n\t"
"kernel_thread_func:	\n\t"
"	popq	%r15	\n\t"
"	popq	%r14	\n\t"	
"	popq	%r13	\n\t"	
"	popq	%r12	\n\t"	
"	popq	%r11	\n\t"	
"	popq	%r10	\n\t"	
"	popq	%r9	\n\t"	
"	popq	%r8	\n\t"	
"	popq	%rbx	\n\t"	
"	popq	%rcx	\n\t"	
"	popq	%rdx	\n\t"	
"	popq	%rsi	\n\t"	
"	popq	%rdi	\n\t"	
"	popq	%rbp	\n\t"	
"	popq	%rax	\n\t"	
"	movq	%rax,	%ds	\n\t"
"	popq	%rax		\n\t"
"	movq	%rax,	%es	\n\t"
"	popq	%rax		\n\t"
"	addq	$0x38,	%rsp	\n\t"
/////////////////////////////////
"	movq	%rdx,	%rdi	\n\t"
"	callq	*%rbx		\n\t"
"	movq	%rax,	%rdi	\n\t"
"	callq	do_exit		\n\t"
);

