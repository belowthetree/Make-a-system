#ifndef GRAPH_H
#define GRAPH_H

#define WHITE 	0x00ffffff		//白
#define BLACK 	0x00000000		//黑
#define RED	    0x00ff0000		//红
#define ORANGE	0x00ff8000		//橙
#define YELLOW	0x00ffff00		//黄
#define GREEN	0x0000ff00		//绿
#define BLUE	0x000000ff		//蓝
#define INDIGO	0x0000ffff		//靛
#define PURPLE	0x008000ff		//紫

struct SCREEN{
	int XPosition, YPosition;
	int XResolution, YResolution;
	int XCharSize, YCharSize;
	unsigned int* FB_addr;
} Pos;

void InitGraph(unsigned long* vbe_addr);
void TranslateX(int x);
unsigned long GetPixel(int x, int y);
void SetPixel(int x, int y, unsigned long color);




























#endif