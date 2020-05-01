#include "abnormal.h"
#include "io.h"

void set_gate(unsigned char* addr, int attr, int ist, void* func);
void set_sys_gate(int n, int ist, void* func);
void set_intr_gate(int n, int ist, void* func);

void InitAbnormal(){
	IDT_Table = (struct GATE*)INTVECBASE;
	LIDT(IDT_Table);
}

void SolveErrorInt(unsigned long rsp, unsigned long error_code)
{
	unsigned long * p = (unsigned long *)(rsp + 0x98);
	if (error_code == 0)
		printf("#DE divide_error(0), RSP:%uX 	RIP:%uX\n", rsp, *p);
	else if(error_code == 1)
		printf("#DB debug_error(1), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 2)
		printf("-- nmi_error(2), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 3)
		printf("#BP INT3(3), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 4)
		printf("#OF overflow(4), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 5)
		printf("#BR bound_error(5), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 6)
		printf("#UD undefine_code(6), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 7)
		printf("#NM FPU_missing(7), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 8)
		printf("#DF double_error(8), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 9)
		printf("-- xiechuliqiyuejie(9), RSP:%X 	RIP:%X\n", rsp, *p);
	else if (error_code == 10)
		printf("#TS invalid_TSS(10), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 11)
		printf("#NP desc_missing(11), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 12)
		printf("#SS SS_error(12), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 13)
		printf("#GP general_protection(13), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 14)
		printf("#PF page_error(14), RSP:%X 	RIP:%X\n", rsp, *p);
	// else if(error_code == 15)	Intel 保留
	// 	printf("#DB debug_error(15), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 16)
		printf("#MF x87 FPU_error_compute_error(16), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 17)
		printf("#AC alignment_check(17), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 18)
		printf("#MC machine_check(18), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 19)
		printf("#XM SIMD_exception(19), RSP:%X 	RIP:%X\n", rsp, *p);
	else if(error_code == 20)
		printf("#VE virtualization_exception(20), RSP:%X 	RIP:%X\n", rsp, *p);
	else
		printf("unknown error, RSP:%X 	RIP:%X\n", rsp, *p);
	while(1);
}

void set_intr_gate(int n, int ist, void* func){
	set_gate((unsigned char*)(IDT_Table + n), 0x8F, ist, func);
}

void set_sys_gate(int n, int ist, void* func){
	set_gate((unsigned char*)(IDT_Table + n), 0xEF, ist, func);
}

void set_gate(unsigned char* addr, int attr, int ist, void* func){
	*((short *)addr) = (unsigned long)(func) & 0xffff;
	*((short *)(addr + 2)) = 0x08;
	//*(addr + 4) = 0;	
	*(addr + 4) = (ist & 0xff);//TODO
	*(addr + 5) = (attr & 0xff);
	*((short *)(addr + 6)) = ((unsigned long)func >> 16) & 0xffff;
	*((uint32 *)(addr + 10)) = ((unsigned long)func >> 48) & 0xffffffff;
}

void LIDT(unsigned long* addr){
	IDT_Pointer.addr = addr;
	IDT_Pointer.limit = 256*16 - 1;
	__asm__ __volatile__(
		"lidt (%%rax)\n\t"
		:
		:"a"(&IDT_Pointer));
}

void do_invalid_TSS(uint64 rsp, uint64 error_code)
{
	uint64 *p = (uint64 *)(rsp + 0x98);

	printf_color(BLACK, RED, "do_invalid_TSS(10),ERROR_CODE:%X,RSP:%X,RIP:%X\n",error_code , rsp , *p);

	if(error_code & 0x01)
		printf_color(BLACK, RED, "The exception occurred during \
			delivery of an event external to the program,\
			such as an interrupt or an earlier exception.\n");

	if(error_code & 0x02)
		printf_color(BLACK, RED, "Refers to a gate descriptor in the IDT;\n");
	else
		printf_color(BLACK, RED, "Refers to a descriptor in the GDT or the current LDT;\n");

	if((error_code & 0x02) == 0)
		if(error_code & 0x04)
			printf_color(BLACK, RED, "Refers to a segment or gate descriptor in the LDT;\n");
		else
			printf_color(BLACK, RED, "Refers to a descriptor in the current GDT;\n");

	printf_color(BLACK, RED, "Segment Selector Index:%010X\n",error_code & 0xfff8);

	while(1);
}

void do_page_fault(uint64 rsp,uint64 error_code)
{
	unsigned long * p = 0;
	unsigned long cr2 = 0;

	__asm__	__volatile__("movq	%%cr2,	%0":"=r"(cr2)::"memory");

	p = (unsigned long *)(rsp + 0x98);
	printf_color(BLACK, RED, "do_page_fault(14),ERROR_CODE: %X,RSP: %X,RIP: %X\n",error_code , rsp , *p);

	if(!(error_code & 0x01))
		printf_color(BLACK, RED, "Page Not-Present,\t");

	if(error_code & 0x02)
		printf_color(BLACK, RED, "Write Cause Fault,\t");
	else
		printf_color(BLACK, RED, "Read Cause Fault,\t");

	if(error_code & 0x04)
		printf_color(BLACK, RED, "Fault in user(3)\t");
	else
		printf_color(BLACK, RED, "Fault in supervisor(0,1,2)\t");

	if(error_code & 0x08)
		printf_color(BLACK, RED, ",Reserved Bit Cause Fault\t");

	if(error_code & 0x10)
		printf_color(BLACK, RED, ",Instruction fetch Cause Fault");

	printf_color(BLACK, RED, "\n");

	printf_color(BLACK, RED, "CR2: %X\n",cr2);

	while(1);
}

void do_nmi(unsigned long rsp,unsigned long error_code)
{
	unsigned long * p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	printf_color(BLACK, RED, "do_nmi(2),ERROR_CODE:%X,RSP:%X,RIP:%X\n",error_code , rsp , *p);
	while(1);
}

void do_double_fault(unsigned long rsp,unsigned long error_code)
{
	unsigned long * p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	printf_color(BLACK, RED, "do_double_fault(8),ERROR_CODE:%018X,RSP:%018X,RIP:%018X\n",error_code , rsp , *p);
	while(1);
}

void do_coprocessor_segment_overrun(unsigned long rsp,unsigned long error_code)
{
	unsigned long * p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	printf_color(BLACK, RED, "do_coprocessor_segment_overrun(9),ERROR_CODE:%018X,RSP:%018X,RIP:%018X\n",error_code , rsp , *p);
	while(1);
}

void do_segment_not_present(unsigned long rsp,unsigned long error_code)
{
	unsigned long * p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	printf_color(BLACK, RED, "do_segment_not_present(11),ERROR_CODE:%018X,RSP:%018X,RIP:%018X\n",error_code , rsp , *p);

	if(error_code & 0x01)
		printf_color(BLACK, RED, "The exception occurred during delivery of an event external to the program,such as an interrupt or an earlier exception.\n");

	if(error_code & 0x02)
		printf_color(BLACK, RED, "Refers to a gate descriptor in the IDT;\n");
	else
		printf_color(BLACK, RED, "Refers to a descriptor in the GDT or the current LDT;\n");

	if((error_code & 0x02) == 0)
		if(error_code & 0x04)
			printf_color(BLACK, RED, "Refers to a segment or gate descriptor in the LDT;\n");
		else
			printf_color(BLACK, RED, "Refers to a descriptor in the current GDT;\n");

	printf_color(BLACK, RED, "Segment Selector Index:%010X\n",error_code & 0xfff8);

	while(1);
}

void do_general_protection(unsigned long rsp,unsigned long error_code)
{
	unsigned long * p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	printf_color(BLACK, RED, "do_general_protection(13),ERROR_CODE:%X,RSP:%X,RIP:%X\n",error_code , rsp , *p);

	if(error_code & 0x01)
		printf_color(BLACK, RED, "The exception occurred during delivery of an event external to the program,such as an interrupt or an earlier exception.\n");

	if(error_code & 0x02)
		printf_color(BLACK, RED, "Refers to a gate descriptor in the IDT;\n");
	else
		printf_color(BLACK, RED, "Refers to a descriptor in the GDT or the current LDT;\n");

	if((error_code & 0x02) == 0)
		if(error_code & 0x04)
			printf_color(BLACK, RED, "Refers to a segment or gate descriptor in the LDT;\n");
		else
			printf_color(BLACK, RED, "Refers to a descriptor in the current GDT;\n");

	printf_color(BLACK, RED, "Segment Selector Index:%010X\n",error_code & 0xfff8);

	while(1);
}