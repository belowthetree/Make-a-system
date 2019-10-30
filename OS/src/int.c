#include "int.h"
#include "main.h"
#include "tool.h"

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
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) GDTBASE;
    int i;

    for (i = 0;i < 8192;i++){
        SetGDT(gdt + i, 0, 0, 0);
    }
    SetGDT(gdt + 1, 0x0, 0xffffffff, 0x4092);
    SetGDT(gdt + 2, KERNELBASE, 0x300000, 0x409a);

    return;
}

void InitIDT()
{
    int i;
    struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) IDTBASE;

    for (i = 0;i < 256;i++){
        SetGate(idt + i, (int) register_clock, 1*8, DA_386IGate);
    }

    load_idtr(0x7ff, IDTBASE);
    return;
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
