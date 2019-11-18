#include "desc.h"
#ifndef _PROCESS_H
#define _PROCESS_H
#define MAX_TASKS
#define PRIVILEGE_TASK 1
#define RPL_TASK 2

typedef struct p_stack_frame{
    int gs;
    int fs;
    int es;
    int ds;
    int edi;
    int esi;
    int ebp;
    int kernel_esp; // popad会忽略这个
    int ebx;
    int edx;
    int ecx;
    int eax;
    int retaddr;    // kernel.asm::save()
    int eip;
    int cs;
    int eflags;
    int esp;
    int ss;
}STACK_FRAME;

typedef struct p_process{
    STACK_FRAME regs;

    short ldt_sel;
    struct SEGMENT_DESCRIPTOR ldts[2];
    int pid;
    char process_name[16];
}PROCESS;

PROCESS PCB[MAX_TASKS];

void TestA();

void InitProcess();

#endif