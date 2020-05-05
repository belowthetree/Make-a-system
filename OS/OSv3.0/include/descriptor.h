#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H
#include "type.h"

#define GDTBASE 0xffff800000030000
#define IDTBASE 0xffff800000031000
#define TSSINDEX 10
#define GDTNUM 20

struct GDT_NORMAL{
	unsigned short limit;
	unsigned char low[3];
	unsigned short attr;
	unsigned char high;
}__attribute__((packed));

struct GDT_TSS{
	unsigned short limit;
	unsigned char low[3];
	unsigned short attr;
	unsigned char high[5];
	unsigned int rev;
}__attribute__((packed));

struct IDT{
	unsigned short low;
	unsigned short selector;
	unsigned char ist;
	unsigned char attr;
	unsigned short mid;
	unsigned int high;
	int rev;
}__attribute__((packed));

struct TSS{
	unsigned int rev1;
	unsigned long rsp0;
	unsigned long rsp1;
	unsigned long rsp2;
	unsigned long rev2;
	unsigned long ist1;
	unsigned long ist2;
	unsigned long ist3;
	unsigned long ist4;
	unsigned long ist5;
	unsigned long ist6;
	unsigned long ist7;
	unsigned int rev3[3];
}__attribute__((packed));

struct POINTER{
	unsigned int limit;
	unsigned long addr;
}__attribute__((packed));

struct GDT_NORMAL* GDT_Table;
struct IDT* IDT_Table;
struct TSS TSS_Table;

struct POINTER IDT_Pointer, GDT_Pointer;

void InitDescripter();

void set_sys_gate(int n, int ist, void* func);
void set_intr_gate(int n, int ist, void* func);
void set_trap_gate(int n, int ist, void* func);

void set_tss(uint64 rsp0, uint64 rsp1, uint64 rsp2, uint64 ist1, uint64 ist2, uint64 ist3, 
	uint64 ist4, uint64 ist5, uint64 ist6, uint64 ist7);

void set_tss_at(int n, uint64 rsp);























#endif