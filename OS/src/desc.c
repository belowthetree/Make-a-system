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

    io_out8(0x21, 0x0);
    io_delay();
    io_out8(0xa1, 0x0);
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

    //InitTSS();
    load_gdtr(0x77, (int)gdt);

    return;
}

void InitIDT()
{
    int i;
    
    for(i = 0;i < 256;i++) {
        interrupt_handlers[i] = 0;
    }

    for (i = 0;i < 256;i++){
        SetGate(idt + i, (int) register_clock, 8, DA_386IGate);
    }
    
    SetGate(idt + IRQ0, (int)irq0, 1*8, DA_386IGate);
    SetGate(idt + IRQ1, (int)irq1, 1*8, DA_386IGate);
    SetGate(idt + IRQ2, (int)irq2, 1*8, DA_386IGate);
    SetGate(idt + IRQ3, (int)irq3, 1*8, DA_386IGate);
    SetGate(idt + IRQ4, (int)irq4, 1*8, DA_386IGate);
    SetGate(idt + IRQ5, (int)irq5, 1*8, DA_386IGate);
    SetGate(idt + IRQ6, (int)irq6, 1*8, DA_386IGate);
    SetGate(idt + IRQ7, (int)irq7, 1*8, DA_386IGate);
    SetGate(idt + IRQ8, (int)irq8, 1*8, DA_386IGate);
    SetGate(idt + IRQ9, (int)irq9, 1*8, DA_386IGate);
    SetGate(idt + IRQ10, (int)irq10, 1*8, DA_386IGate);
    SetGate(idt + IRQ11, (int)irq11, 1*8, DA_386IGate);
    SetGate(idt + IRQ12, (int)irq12, 1*8, DA_386IGate);
    SetGate(idt + IRQ13, (int)irq13, 1*8, DA_386IGate);
    SetGate(idt + IRQ14, (int)irq14, 1*8, DA_386IGate);
    SetGate(idt + IRQ15, (int)irq15, 1*8, DA_386IGate);
    
    load_idtr(0x7ff, (int)idt);
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
    printi(2, 1);
    while(1);
    if (interrupt_handlers[regs->int_num]){
        interrupt_handlers[regs->int_num](regs);
    } else {
        char msg[] = "Unhandle interrupt: ";
        prints(msg);
        printi(regs->int_num, 1);
    }
    return;
}

void irq_handler(pt_regs *regs)
{
    // 发送中断结束信号给 PICs
    // 按照我们的设置，从 32 号中断起为用户自定义中断
    // 因为单片的 Intel 8259A 芯片只能处理 8 级中断 
    // 故大于等于 40 的中断号是由从片处理的
    
    if (regs->int_num >= 40) {
        // 发送重设信号给从片
        io_out8(0xa0, 0x20);
    }
    // 发送重设信号给主片
    io_out8(0x20, 0x20);

    if (interrupt_handlers[regs->int_num]) {
        interrupt_handlers[regs->int_num](regs);
    }
    return;
}

void isr0_handler(pt_regs * regs)
{
    char msg[] = "Interrupt 0";
    prints(msg);
    printi(regs->int_num, 1);
    return;
}

void register_interrupt_handler(int irq, void* func) // int func
{
    interrupt_handlers[irq] = func;
    printi(interrupt_handlers[irq],1);
    return ;
}