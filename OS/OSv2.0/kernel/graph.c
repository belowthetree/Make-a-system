#include "graph.h"
#include "memory.h"

void SetPixel(int x, int y, uint32 color){
    if (y >= Pos.YResolution || x >= Pos.XResolution || x < 0 || y < 0)
        return;
    int yy = Pos.XResolution * y;
    Pos.FB_addr[yy + x] = color;
}

uint32 GetPixel(int x, int y){
    return Pos.FB_addr[y * Pos.XResolution + x];
}

void InitGraph(uint32 *addr)
{
    Pos.FB_addr = addr;
    Pos.XPosition = Pos.YPosition = 0;
    Pos.XResolution = 1440;
    Pos.YResolution = 900;
    Pos.YCharSize = 16;
    Pos.XCharSize = 8;

    Pos.FB_length = (Pos.XResolution * Pos.YResolution * 4 + 
        PAGE_4K_SIZE - 1) & (PAGE_4K_MASK);
}

void TranslateX(int x)
{
    Pos.XPosition += x;
    Pos.YPosition += (Pos.XPosition / Pos.XResolution) * Pos.YCharSize;
    Pos.XPosition %= Pos.XResolution;
    Pos.YPosition %= Pos.YResolution;
}

void frame_buffer_init()
{
    // 重新映射 VBE
    unsigned long i;
    unsigned long * tmp;
    unsigned long * tmp1;
    unsigned int * FB_addr = (unsigned int *)Phy_To_Virt(0xe0000000);

    Global_CR3 = Get_gdt();
}
