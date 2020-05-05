#include "descriptor.h"
#include "io.h"
#include "lib.h"

void set_gdt(int n, uint32 addr, uint16 limit, uint16 attr);
void set_idt(int n, uint8 attr, uint8 ist, uint64 func);
void set_gdt_tss(int n, uint64 addr, uint16 limit, uint16 attr);

void InitDescripter(){
	printf("init descriptor\n");
	GDT_Table = (struct GDT_NORMAL*)GDTBASE;
	IDT_Table = (struct IDT*)IDTBASE;

	GDT_Pointer.limit = GDTNUM * sizeof(struct GDT_NORMAL) - 1;
	GDT_Pointer.addr = (uint64)GDTBASE;
	IDT_Pointer.limit = 256 * sizeof(struct IDT) - 1;
	IDT_Pointer.addr = (uint64)IDTBASE;

	memset((void *)GDT_Table, 0, sizeof(struct GDT_NORMAL) * GDTNUM);
	memset((void *)IDT_Table, 0, sizeof(struct IDT) * 256);
	set_gdt(1, 0, 0, 0x2098);	// 内核代码段
	set_gdt(2, 0, 0, 0x0092);	// 内核数据段
	set_gdt(5, 0, 0, 0x20f8);	// 用户代码段
	set_gdt(6, 0, 0, 0x00f2);	// 用户代码段

	set_gdt_tss(TSSINDEX, (uint64)&TSS_Table, 103, 0x89);	// TSS 段

	load_gdtr(GDT_Pointer.limit, GDT_Pointer.addr);
	load_idtr(IDT_Pointer.limit, IDT_Pointer.addr);

	set_tss(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00,
     0xffff800000008e00, 0xffff800000008e00, 0xffff800000008e00, 
     0xffff800000008e00, 0xffff800000008e00, 0xffff800000008e00, 
     0xffff800000008e00);
	load_TR(10);
}

void set_tss_at(int n, uint64 rsp){
	switch (n){
		case 1:
			TSS_Table.ist1 = rsp;
			break;
		case 2:
			TSS_Table.ist2 = rsp;
			break;
		case 3:
			TSS_Table.ist3 = rsp;
			break;
		case 4:
			TSS_Table.ist4 = rsp;
			break;
		case 5:
			TSS_Table.ist5 = rsp;
			break;
		case 6:
			TSS_Table.ist6 = rsp;
			break;
		case 7:
			TSS_Table.ist7 = rsp;
			break;
		default:
			printf_color(BLACK, RED, "set tss num error %d\n", n);
	}
}

void set_tss(uint64 rsp0, uint64 rsp1, uint64 rsp2, uint64 ist1, uint64 ist2, uint64 ist3, 
	uint64 ist4, uint64 ist5, uint64 ist6, uint64 ist7){
	TSS_Table.rsp0 = rsp0;
	TSS_Table.rsp1 = rsp1;
	TSS_Table.rsp2 = rsp2;
	TSS_Table.ist1 = ist1;
	TSS_Table.ist2 = ist2;
	TSS_Table.ist3 = ist3;
	TSS_Table.ist4 = ist4;
	TSS_Table.ist5 = ist5;
	TSS_Table.ist6 = ist6;
	TSS_Table.ist7 = ist7;
}

void set_gdt_tss(int n, uint64 addr, uint16 limit, uint16 attr){
	struct GDT_TSS *ts = (struct GDT_TSS*)(&GDT_Table[n]);
	ts->limit = limit;
	ts->low[0] = (uint8)(addr & 0xff);
	ts->low[1] = (uint8)(addr >> 8) & 0xff;
	ts->low[2] = (uint8)(addr >> 16) & 0xff;
	ts->attr = attr;
	ts->high[0] = (uint8)(addr >> 24) & 0xff;
	ts->high[1] = (uint8)(addr >> 32) & 0xff;
	ts->high[2] = (uint8)(addr >> 40) & 0xff;
	ts->high[3] = (uint8)(addr >> 48) & 0xff;
	ts->high[4] = (uint8)(addr >> 56) & 0xff;
}

void set_gdt(int n, uint32 addr, uint16 limit, uint16 attr){
	GDT_Table[n].limit = limit;
	GDT_Table[n].low[0] = (uint8)(addr & 0xff);
	GDT_Table[n].low[0] = (uint8)(addr >> 8) & 0xff;
	GDT_Table[n].low[0] = (uint8)(addr >> 16) & 0xff;
	GDT_Table[n].attr = attr;
	GDT_Table[n].high = (uint8)(addr >> 24) & 0xff;
}

void set_idt(int n, uint8 attr, uint8 ist, uint64 func){
	IDT_Table[n].low = (uint16)(func & 0xffff);
	IDT_Table[n].mid = (uint16)(func >> 16) & 0xffff;
	IDT_Table[n].high = (uint32)(func >> 32) & 0xffffffff;
	IDT_Table[n].selector = 0x0008;
	IDT_Table[n].ist = ist;
	IDT_Table[n].attr = attr;
}

void set_sys_gate(int n, int ist, void* func){
	set_idt(n, 0xef, ist, (uint64)func);
}

void set_intr_gate(int n, int ist, void* func){
	set_idt(n, 0x8e, ist, (uint64)func);
}

void set_trap_gate(int n, int ist, void* func){
	set_idt(n, 0x8f, ist, (uint64)func);
}