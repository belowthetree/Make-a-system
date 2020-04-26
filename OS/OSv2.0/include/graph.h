#ifndef _GRAPH_H
#define _GRAPH_H
#include "type.h"

#define WHITE 	0x00ffffff		//白
#define BLACK 	0x00000000		//黑
#define RED	    0x00ff0000		//红
#define ORANGE	0x00ff8000		//橙
#define YELLOW	0x00ffff00		//黄
#define GREEN	0x0000ff00		//绿
#define BLUE	0x000000ff		//蓝
#define INDIGO	0x0000ffff		//靛
#define PURPLE	0x008000ff		//紫

struct position{
    // 屏幕分辨率
    int XResolution;
    int YResolution;
    // 光标坐标
    int XPosition;
    int YPosition;
    // 字符大小
    int XCharSize;
    int YCharSize;
    // 帧缓存地址、容量
    uint32 * FB_addr;
    unsigned long * FB_length;
}Pos;

void InitGraph(uint32 *addr);
void TranslateX(int x);

void SetPixel(int x, int y, uint32 color);
uint32 GetPixel(int x, int y);

#endif
