#include "graph.h"

void InitGraph(uint32 *addr)
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
