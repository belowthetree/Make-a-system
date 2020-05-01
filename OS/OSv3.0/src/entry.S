# #include "linkage.h"

# R15	=	0x00
# R14	=	0x08
# R13	=	0x10
# R12	=	0x18
# R11	=	0x20
# R10	=	0x28
# R9	=	0x30
# R8	=	0x38
# RBX	=	0x40
# RCX	=	0x48
# RDX	=	0x50
# RSI	=	0x58
# RDI	=	0x60
# RBP	=	0x68
# DS	=	0x70
# ES	=	0x78
# RAX	=	0x80
# FUNC	=	0x88
# ERRCODE	=	0x90
# RIP	=	0x98
# CS	=	0xa0
# RFLAGS	=	0xa8
# OLDRSP	=	0xb0
# OLDSS	=	0xb8

# RESTORE_ALL:
# 	popq	%r15;
# 	popq	%r14;
# 	popq	%r13;
# 	popq	%r12;
# 	popq	%r11;
# 	popq	%r10;
# 	popq	%r9;
# 	popq	%r8;
# 	popq	%rbx;
# 	popq	%rcx;
# 	popq	%rdx;
# 	popq	%rsi;		
# 	popq	%rdi;
# 	popq	%rbp;
# 	popq	%rax;
# 	movq	%rax,	%ds;
# 	popq	%rax;
# 	movq	%rax,	%es;
# 	popq	%rax;
# 	addq	$0x10,	%rsp;
# 	iretq;

# #define GET_CURRENT(reg)	\
# 	movq	$-32768,reg;	\
# 	andq	%rsp,	reg

# ret_from_exception:
# 	/*GET_CURRENT(%ebx)	need rewrite*/
# ENTRY(ret_from_intr)
# 	jmp	RESTORE_ALL	/*need rewrite*/


# ENTRY(divide_error)
# 	pushq	$0
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)

# // 系统错误现场保存
# error_code:
# 	pushq	%rax
# 	movq	%es,	%rax
# 	pushq	%rax
# 	movq	%ds,	%rax
# 	pushq	%rax
# 	xorq	%rax,	%rax

# 	pushq	%rbp
# 	pushq	%rdi
# 	pushq	%rsi
# 	pushq	%rdx
# 	pushq	%rcx
# 	pushq	%rbx
# 	pushq	%r8
# 	pushq	%r9
# 	pushq	%r10
# 	pushq	%r11
# 	pushq	%r12
# 	pushq	%r13
# 	pushq	%r14
# 	pushq	%r15	
	
# 	cld
# 	movq	ERRCODE(%rsp),	%rsi
# 	movq	FUNC(%rsp),	%rdx	

# 	movq	$0x10,	%rdi
# 	movq	%rdi,	%ds
# 	movq	%rdi,	%es

# 	movq	%rsp,	%rdi
# 	////GET_CURRENT(%ebx)

# 	callq 	*%rdx

# 	jmp	ret_from_exception	

# ENTRY(debug)
# 	pushq	$1
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(nmi)
# 	pushq	%rax
# 	cld;
# 	pushq	%rax;	
	
# 	pushq	%rax
# 	movq	%es,	%rax
# 	pushq	%rax
# 	movq	%ds,	%rax
# 	pushq	%rax
# 	xorq	%rax,	%rax
	
# 	pushq	%rbp;		
# 	pushq	%rdi;		
# 	pushq	%rsi;		
# 	pushq	%rdx;		
# 	pushq	%rcx;		
# 	pushq	%rbx;		
# 	pushq	%r8;		
# 	pushq	%r9;		
# 	pushq	%r10;		
# 	pushq	%r11;		
# 	pushq	%r12;		
# 	pushq	%r13;		
# 	pushq	%r14;		
# 	pushq	%r15;
	
# 	movq	$0x10,	%rdx;	
# 	movq	%rdx,	%ds;	
# 	movq	%rdx,	%es;
	
# 	movq	$0,	%rsi
# 	movq	%rsp,	%rdi

# 	callq	do_nmi

# 	jmp	RESTORE_ALL

# ENTRY(int3)
# 	pushq	$3
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(overflow)
# 	pushq	$4
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(bounds)
# 	pushq	$5
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(undefined_opcode)
# 	pushq	$6
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(dev_not_available)	/* need rewrite */
# 	pushq	$7
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code


# ENTRY(double_fault)
# 	pushq	%rax
# 	leaq	do_double_fault(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(coprocessor_segment_overrun)
# 	pushq	$9
# 	pushq	%rax
# 	leaq	do_coprocessor_segment_overrun(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(invalid_TSS)
# 	pushq	%rax
# 	leaq	do_invalid_TSS(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(segment_not_present)
# 	pushq	%rax
# 	leaq	do_segment_not_present(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(stack_segment_fault)
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(general_protection)
# 	pushq	%rax
# 	leaq	do_general_protection(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(page_fault)
# 	pushq	%rax
# 	leaq	do_page_fault(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(x87_FPU_error)
# 	pushq	$16
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(alignment_check)
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(machine_check)
# 	pushq	$18
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(SIMD_exception)
# 	pushq	$19
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(virtualization_exception)
# 	pushq	$20
# 	pushq	%rax
# 	leaq	SolveErrorInt(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	error_code

# ENTRY(system_call)
# 	sti
# 	subq	$0x38,	%rsp			 
# 	cld;					 

# 	pushq	%rax;				 	
# 	movq	%es,	%rax;			 	
# 	pushq	%rax;				 	
# 	movq	%ds,	%rax;			 	
# 	pushq	%rax;				 	
# 	xorq	%rax,	%rax;			 	
# 	pushq	%rbp;				 	
# 	pushq	%rdi;				 	
# 	pushq	%rsi;				 	
# 	pushq	%rdx;				 	
# 	pushq	%rcx;				 
# 	pushq	%rbx;				 	
# 	pushq	%r8;				 	
# 	pushq	%r9;				 	
# 	pushq	%r10;				 
# 	pushq	%r11;				 
# 	pushq	%r12;				 	
# 	pushq	%r13;				 
# 	pushq	%r14;				 	
# 	pushq	%r15;				 	
# 	movq	$0x10,	%rdx;			 	
# 	movq	%rdx,	%ds;			 	
# 	movq	%rdx,	%es;			 
# 	movq	%rsp,	%rdi			 	
			
# 	# callq	system_call_function

# ENTRY(ret_system_call)			
# 	movq	%rax,	0x80(%rsp)		 
# 	popq	%r15				 
# 	popq	%r14				 	
# 	popq	%r13				 	
# 	popq	%r12				 	
# 	popq	%r11				 	
# 	popq	%r10				 	
# 	popq	%r9				 	
# 	popq	%r8				 	
# 	popq	%rbx				 	
# 	popq	%rcx				 	
# 	popq	%rdx				 	
# 	popq	%rsi				 	
# 	popq	%rdi				 	
# 	popq	%rbp				 	
# 	popq	%rax				 	
# 	movq	%rax,	%ds			 
# 	popq	%rax				 
# 	movq	%rax,	%es			 
# 	popq	%rax		 
# 	addq	$0x38,	%rsp
# 	.byte	0x48
# 	sysexit

# //  中断触发现场保存
# int_code:
# 	pushq	%rax
# 	movq	%es,	%rax
# 	pushq	%rax
# 	movq	%ds,	%rax
# 	pushq	%rax
# 	xorq	%rax,	%rax

# 	pushq	%rbp
# 	pushq	%rdi
# 	pushq	%rsi
# 	pushq	%rdx
# 	pushq	%rcx
# 	pushq	%rbx
# 	pushq	%r8
# 	pushq	%r9
# 	pushq	%r10
# 	pushq	%r11
# 	pushq	%r12
# 	pushq	%r13
# 	pushq	%r14
# 	pushq	%r15	
	
# 	cld
# 	movq	ERRCODE(%rsp),	%rsi
# 	movq	FUNC(%rsp),	%rdx

# 	movq	$0x10,	%rdi
# 	movq	%rdi,	%ds
# 	movq	%rdi,	%es

# 	movq	%rsp,	%rdi
# 	////GET_CURRENT(%ebx)

# 	callq 	*%rdx

# 	jmp	ret_from_exception

# ENTRY(IRQ0x20_interrupt)
# 	pushq	$0x20
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code

# ENTRY(IRQ0x21_interrupt)
# 	pushq	$0x21
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x22_interrupt)
# 	pushq	$0x22
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x23_interrupt)
# 	pushq	$0x23
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x24_interrupt)
# 	pushq	$0x24
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x25_interrupt)
# 	pushq	$0x25
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x26_interrupt)
# 	pushq	$0x26
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x27_interrupt)
# 	pushq	$0x27
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x28_interrupt)
# 	pushq	$0x28
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x29_interrupt)
# 	pushq	$0x29
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x2a_interrupt)
# 	pushq	$0x2a
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x2b_interrupt)
# 	pushq	$0x2b
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x2c_interrupt)
# 	pushq	$0x2c
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x2d_interrupt)
# 	pushq	$0x2d
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x2e_interrupt)
# 	pushq	$0x2e
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x2f_interrupt)
# 	pushq	$0x2f
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x30_interrupt)
# 	pushq	$0x30
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x31_interrupt)
# 	pushq	$0x31
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x32_interrupt)
# 	pushq	$0x32
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x33_interrupt)
# 	pushq	$0x33
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x34_interrupt)
# 	pushq	$0x34
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x35_interrupt)
# 	pushq	$0x35
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
	
# ENTRY(IRQ0x36_interrupt)
# 	pushq	$0x36
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code

# ENTRY(IRQ0x37_interrupt)
# 	pushq	$0x37
# 	pushq	%rax
# 	leaq	do_IRQ(%rip),	%rax
# 	xchgq	%rax,	(%rsp)
# 	jmp	int_code
