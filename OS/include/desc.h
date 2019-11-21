#include "main.h"
#ifndef _DESC_H
#define _DESC_H

#define ICW1    0x011
#define CICW2   0x028
#define MICW2   0x020
#define MICW3   0x004
#define CICW3   0x002
#define ICW4    0x001

#define GDTBASE 0x270000
#define IDTBASE 0x300000
#define KERNELBASE  0x30000

struct SEGMENT_DESCRIPTOR{
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};

struct GATE_DESCRIPTOR{
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};

typedef struct s_tss{
    int backlink;
    int esp0;
    int ss0;
    int esp1;
    int ss1;
    int esp2;
    int ss2;
    int cr3;
    int eip;
    int flags;
    int eax;
    int ecx;
    int edx;
    int ebx;
    int esp;
    int ebp;
    int esi;
    int edi;
    int es;
    int cs;
    int ss;
    int ds;
    int fs;
    int gs;
    int ldt;
    short trap;
    short iobase;
}TSS;

TSS tss;

// 描述符数组
struct SEGMENT_DESCRIPTOR *gdt;
struct GATE_DESCRIPTOR *idt;

// LDT
struct SEGMENT_DESCRIPTOR * Code32;
struct SEGMENT_DESCRIPTOR * KernelData;
struct SEGMENT_DESCRIPTOR * KernelVRAM;
struct SEGMENT_DESCRIPTOR * LDTCode;
struct SEGMENT_DESCRIPTOR * LDTVRAM;
struct SEGMENT_DESCRIPTOR * LDTData;
struct SEGMENT_DESCRIPTOR * Seg_TSS;

#define SelectorCode32          8
#define SelectorKernelVRAM      2*8
#define SelectorKernelData      3*8
#define SelectorLDTCode         4*8 + SA_RPL3
#define SelectorLDTVRAM         5*8 + SA_RPL3
#define SelectorLDTData         6*8 + SA_RPL3
#define SelectorTSS             7*8

void InitPIC();
void InitGDT();
void InitIDT();
void InitTSS();

void SetGDT(struct SEGMENT_DESCRIPTOR *sd, int addr, int limit, int ar);
void SetGate(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

void SetLDT();
void SetLDT(int selector);

void register_interrupt_handler(int irq, void * func);


/* 选择子类型值说明 */
/* 其中, SA_ : Selector Attribute */
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4

/* 描述符类型值说明 */
#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节			*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值		*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/

typedef
struct pt_regs_t{
    uint32 ds;
    uint32 edi;
    uint32 esi;
    uint32 ebp;
    uint32 esp;
    uint32 ebx;
    uint32 edx;
    uint32 ecx;
    uint32 eax;
    uint32 int_num;
    uint32 error_code;
    uint32 ip;
    uint32 cs;
    uint32 eflags;
    uint32 useresp;
    uint32 ss;
} pt_regs;

void isr_handler(pt_regs *pt_regs);

// 中断函数数组
void (* interrupt_handlers[256])(pt_regs *);

void isr0_handler(pt_regs *);

// 异常
void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();

// intel 保留
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

#define IRQ0    32  // 电脑系统计时器
#define IRQ1    33  // 键盘
#define IRQ2    34  // 与 IRQ9 相接，MPU-401 MD使用
#define IRQ3    35  // 串口设备
#define IRQ4    36  // 串口设备
#define IRQ5    37  // 建议声卡使用
#define IRQ6    38  // 软驱传输控制使用
#define IRQ7    39  // 打印机传输控制使用
#define IRQ8    40  // 即时时钟
#define IRQ9    41  // 与 IRQ2 相接，可设定给其他硬件
#define IRQ10   42  // 建议网卡使用
#define IRQ11   43  // 建议 AGP 显卡使用
#define IRQ12   44  // 接 PS/2 鼠标，也可设定给其他硬件
#define IRQ13   45  // 协处理器使用
#define IRQ14   46  // IDE0 传输控制使用
#define IRQ15   47  // IDE1 传输控制使用

void irq0();    // 电脑系统计时器
void irq1();    // 键盘
void irq2();    // 。。。。
void irq3(); 
void irq4(); 
void irq5(); 
void irq6(); 
void irq7(); 
void irq8(); 
void irq9(); 
void irq10(); 
void irq11(); 
void irq12(); 
void irq13(); 
void irq14(); 
void irq15();

#endif