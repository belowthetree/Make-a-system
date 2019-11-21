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

    process->regs.cs    = SelectorLDTCode;
	process->regs.ds	= SelectorLDTData;
	process->regs.es	= SelectorLDTData;
	process->regs.fs	= SelectorLDTData;
	process->regs.ss	= SelectorLDTData;
    process->regs.gs    = SelectorLDTVRAM;
    process->regs.eip   = (int) TestA;
    process->regs.esp   = 0x3ff;
	// process->regs.eflags = 0x1202;	// IF=1, IOPL=1, bit 2 is always 1.

    SetVRAM(0, 2, 'B', BLACK, DEEP_RED);
    printi(process->regs.ss, 1);
    printi((int)TestA, 1);

    restart((int) process, (int)process->ldt_sel);
    while(1);
}

void TestA()
{
    SetVRAM(3, 3, 'A', BLACK, DEEP_RED);
    while(1)
        ;
}
