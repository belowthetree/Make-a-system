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
	
	jmp main
