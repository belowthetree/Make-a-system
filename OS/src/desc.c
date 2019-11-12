#include "desc.h"
#include "main.h"
#include "tool.h"
#include "process.h"

void InitPIC()
{
    io_out8(0x20, ICW1);
    io_delay();
    io_out8(0xa0, ICW1);
    io_delay();

    io_out8(0x21, MICW2);
    io_delay();
    io_out8(0xa1, CICW2);
    io_delay();

    io_out8(0x21, MICW3);
    io_delay();
    io_out8(0xa1, CICW3);
    io_delay();

    io_out8(0x21, ICW4);
    io_delay();
    io_out8(0xa1, ICW4);
    io_delay();

    io_out8(0x21, 0xfd);
    io_delay();
    io_out8(0xa1, 0xff);
    io_delay();

    return;
}

void InitGDT()
{
    int i;
    gdt = (struct SEGMENT_DESCRIPTOR *) GDTBASE;
    Code32      = gdt + 1;
    KernelVRAM  = gdt + 2;
    KernelData  = gdt + 3;
    LDTCode     = gdt + 4;
    LDTVRAM     = gdt + 5;
    LDTData     = gdt + 6;
    Seg_TSS     = gdt + 7;

    tss.ss0 = SelectorKernelData;

    for (i = 0;i < 16;i++){
        SetGDT(&gdt[i], 0, 0, 0);
    }
    SetGDT(Code32, 0x0, 0xffffffff, DA_32 | DA_DPL0 | DA_CR);
    SetGDT(KernelVRAM, 0xb8000, 0xffff, DA_DRW | DA_DPL0);
    SetGDT(KernelData, 0x0, 0xffffffff, DA_DRW | DA_32 | DA_DPL0);
    SetGDT(LDTCode, 0x0, 0xffffffff, DA_LDT | DA_DPL1);
    SetGDT(LDTVRAM, 0xb8000, 0xffff, DA_DRW | DA_DPL1);
    SetGDT(LDTData, 0x0, 0xffffffff, DA_DRW | DA_DPL1);
    SetGDT(Seg_TSS, (int) &tss, sizeof(tss) - 1, DA_386TSS);

    InitTSS();

    load_gdtr(0x77, gdt);

    return;
}

void InitIDT()
{
    int i;

    for (i = 0;i < 256;i++){
        SetGate(idt + i, (int) register_clock, 8, DA_386IGate);
    }
    SetGate(idt + 0x21, (int) _inthandler21, 8, DA_386IGate);

    load_idtr(0x7ff, idt);
    return;
}

void InitTSS()
{
    memset((char*) &tss, 0, sizeof(tss));

    tss.ss0 = SelectorKernelData;
    tss.iobase = sizeof(tss);
}

void SetGate(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = 0;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    
    return;
}

void SetGDT(struct SEGMENT_DESCRIPTOR *sd, int addr, int limit, int ar)
{
    if (limit > 0xfffff)
    {
        ar |= 0x8000;
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low = addr & 0xffff;
    sd->base_mid = (addr >> 16) & 0xf;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high = (addr >> 24) & 0xff;

    return;
}

void SetLDT(int selector)
{
    // TODO
}

void isr_handler(pt_regs *regs)
{
    
}
