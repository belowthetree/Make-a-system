#ifndef _INT_H
#define _INT_H

#define ICW1    0x011
#define CICW2   0x028
#define MICW2   0x020
#define MICW3   0x004
#define CICW3   0x002
#define ICW4    0x001

#define GDTBASE 0x270000
#define IDTBASE 0x10000
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

void InitPIC();

void InitGDT();
void InitIDT();
void SetGDT(struct SEGMENT_DESCRIPTOR *sd, int addr, int limit, int ar);

void SetGate(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#endif