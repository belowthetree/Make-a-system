#include "graph.h"

void SetPixel(int x, int y, unsigned long color){
    if (y >= Pos.YResolution || x >= Pos.XResolution || x < 0 || y < 0)
        return;
    int yy = Pos.XResolution * y;
    Pos.FB_addr[yy + x] = color;
}

unsigned long GetPixel(int x, int y){
    return Pos.FB_addr[y * Pos.XResolution + x];
}

void InitGraph(unsigned long *addr)
{
    Pos.FB_addr = addr;
    Pos.XPosition = Pos.YPosition = 0;
    Pos.XResolution = 1440;
    Pos.YResolution = 900;
    Pos.YCharSize = 16;
    Pos.XCharSize = 8;
}

void TranslateX(int x)
{
    Pos.XPosition += x;
    Pos.YPosition += (Pos.XPosition / Pos.XResolution) * Pos.YCharSize;
    Pos.XPosition %= Pos.XResolution;
    Pos.YPosition %= Pos.YResolution;
}