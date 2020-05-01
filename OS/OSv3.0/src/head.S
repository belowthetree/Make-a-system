.section .text

.global _start


_start:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %fs
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %gs
	mov TEST(%rip), %rax
	mov $0xffff800000007c00, %rsp
	mov %rsp, %rbp

TEST:
	.quad 0x10000
	# movq %rax, (0x10000)
	# movq $0x14003, (0x12000)

	# movq $0x000083, (0x14000)
	# movq $0x200083, (0x14000)
	# movq $0x400083, (0x14000)
	# movq $0x600083, (0x14000)
	# movq $0x800083, (0x14000)
	# movq $0xa00083, (0x14000)
	# movq $0xc00083, (0x14000)
	# movq $0xe00083, ($0x14000)
	
	jmp main

