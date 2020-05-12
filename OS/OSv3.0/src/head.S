#include"linkage.h"
.section .text

.global _start


_start:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %fs
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %gs
	movq	EndOfStack(%rip),	%rsp
	
	movq	go_to_kernel(%rip),	%rax		/* movq address */
	pushq	$0x08
	pushq	%rax
	lretq

go_to_kernel:
	.quad	main

ENTRY(_stack_start)
	.fill	4096, 1, 0
EndOfStack:
	.quad   _stack_start + 4096