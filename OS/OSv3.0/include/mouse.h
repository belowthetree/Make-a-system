#ifndef MOUSE_H
#define MOUSE_H

#define KBCMD_SENDTO_MOUSE	0xd4		// 发送数据给鼠标
#define MOUSE_ENABLE		0xf4		// 允许鼠标发送数据
#define KBCMD_EN_MOUSE_INTFACE	0xa8 	// 开启鼠标端口

#define EDGECOLOR 0xaaaaaa
#define CONTENT	  0xff0000

void InitMouse();
void mouse_handler();
char get_mouse_code();
void decode_mouse();

struct MOUSE_BUF{
	int byte0, byte1, byte2;
};

struct MOUSE_INFO
{
	int xPos, yPos;
	unsigned long color;
} pointer;

int mouse_left, mouse_right, mouse_mid;






















#endif