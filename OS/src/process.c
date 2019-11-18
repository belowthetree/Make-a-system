#include "process.h"
#include "desc.h"
#include "main.h"
#include "tool.h"

void InitProcess()
{
    PROCESS * process = PCB;

    process->ldt_sel = SelectorLDTCode;
    process->ldts[0] = gdt[SelectorLDTCode >> 3];
    // process->ldts[0].access_right = DA_C | PRIVILEGE_TASK << 5;
    process->ldts[1] = gdt[SelectorLDTData >> 3];
    // process->ldts[1].access_right = DA_DRW | PRIVILEGE_TASK << 5;

    process->regs.cs    = 4*8;
	process->regs.ds	= 6*8;
	process->regs.es	= 6*8;
	process->regs.fs	= 6*8;
	process->regs.ss	= 6*8;
    process->regs.gs    = 5*8;
    process->regs.eip   = (int) TestA;
    process->regs.esp   = (int) Stack;
	process->regs.eflags = 0x1202;	// IF=1, IOPL=1, bit 2 is always 1.

    SetVRAM(0, 2, 'B', BLACK, DEEP_RED);
    printi(process, 1);
    printi((int)TestA, 1);
    TestA();
    restart((int) process, (int)process->ldt_sel);
}

void TestA()
{
	int i = 0;
    SetVRAM(0, 0, 'A', BLACK, DEEP_RED);
	while(1){
		io_hlt();
	}
}
