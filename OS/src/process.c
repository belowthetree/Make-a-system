#include "process.h"
#include "desc.h"
#include "main.h"

void InitProcess()
{
    PROCESS * process = PCB;

    process->ldt_sel = SelectorLDTCode;
    process->ldts[0] = gdt[SelectorLDTCode >> 3];
    process->ldts[0].access_right = DA_C | PRIVILEGE_TASK << 5;
    process->ldts[1] = gdt[SelectorLDTData >> 3];
    process->ldts[1].access_right = DA_DRW | PRIVILEGE_TASK << 5;

    process->regs.cs    = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
	process->regs.ds	= (2*8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
	process->regs.es	= (2*8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
	process->regs.fs	= (2*8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
	process->regs.ss	= (2*8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    process->regs.gs    = (SelectorKernelVRAM & SA_RPL_MASK) | RPL_TASK;
    process->regs.eip   = (int) TestA;
    process->regs.esp   = (int) Stack;
	process->regs.eflags = 0x1202;	// IF=1, IOPL=1, bit 2 is always 1.

    SetVRAM(0, 2, 'B', BLACK, DEEP_RED);
    restart((int) &process, (int)process->ldt_sel);
}

void TestA()
{
	int i = 0;
	while(1){
		SetVRAM(i++, 0, 'A', BLACK, WHITE);
        int k = 0;
        for (k = 0;k < 102400;k++)
            ;
	}
}
